#include "controller/ticket/TicketController.h"
#include <pqxx/pqxx>
#include "utils/response.h"
#include "utils/Config.h"
#include "utils/uuid.h"
#include "utils/response.h"

TicketController::TicketController()
{
    REST_HANDLER(TicketController, "", "POST", createTicket);
    REST_HANDLER(TicketController, "/update", "POST", updateTicket);
    REST_HANDLER(TicketController, "", "GET", searchTicket);
    REST_HANDLER(TicketController, "", "DELETE", deleteTicket);
}

response TicketController::createTicket(const request &req)
{
    CROW_LOG_INFO << req.body;

    auto body = json::load(req.body);

    if(!body || !body.has("userId") || !body.has("eventId") || !body.has("price"))
    {
        return json::wvalue{{"error", "please summit all fields"}};
    }

    std::string userId = body["userId"].s();
    std::string eventId = body["eventId"].s();
    std::string price = body["price"].s();
    std::string id = generateUUID();

    pqxx::connection conn(Config::get("database"));

    pqxx::work w{conn};

    std::string query = R"(
        insert into "Tickets"
        (id, "userId", "eventId", "price", "createdAt", "updatedAt")
        values ($1, $2, $3, $4, NOW(), NOW())
        returning *;
    )";

    auto r = w.exec_params(query, id, userId, eventId, price);

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response TicketController::updateTicket(const request &req)
{
    CROW_LOG_INFO << req.body;

    auto body = json::load(req.body);

    if(!body || !body.has("userId") || !body.has("eventId") || !body.has("price") || !body.has("id"))
    {
        return json::wvalue{{"error", "please summit all fields"}};
    }

    for(const auto& key : body.keys())
    {
        if(body[key] == "")
        {
            CROW_LOG_INFO << key << " is empty";
            return json::wvalue{{"error", key + " is empty"}};
        }
    }

    std::string userId = body["userId"].s();
    std::string eventId = body["eventId"].s();
    std::string price = body["price"].s();
    std::string id = body["id"].s();

    pqxx::connection conn(Config::get("database"));

    pqxx::work w{conn};

    std::string query = R"(
        update "Tickets" set
        ("userId", "eventId", "price", "updatedAt")
        values ($1, $2, $3, NOW())
        where id = $4
        returning *;
    )";

    auto r = w.exec_params(query,  userId, eventId, price, id);

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response TicketController::searchTicket(const request &req)
{
    CROW_LOG_INFO << req.body;

    std::string title = req.url_params.get("title");
    
    pqxx::connection conn(Config::get("database"));

    pqxx::work w{conn};

    std::string query = R"(
        select * from "Tickets"
        where title like $1
    )";

    auto r = w.exec_params(query, "%" + title + "%");
    auto data = resultsToJSON(r);

    w.commit();

    return json::wvalue{{"message", "ok"}, {"data", data}};
    
}


response TicketController::deleteTicket(const request &req)
{
    
    CROW_LOG_INFO << req.body;

    auto body = json::load(req.body);
    if(!body || !body.has("id"))
    {
        return json::wvalue{{"error", "no id"}};
    }

    std::string id = body["id"].s();

    pqxx::connection conn(Config::get("database"));

    pqxx::work w{conn};

    std::string query = R"(
        delete from "Tickets"
        where id = $1
        returning *;
    )";

    auto r = w.exec_params(query, id);

    if (r.empty())
    {
        return json::wvalue{{"error", "Ticket not found"}};
    }

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "Ticket deleted"}, {"data", data}};
    
}