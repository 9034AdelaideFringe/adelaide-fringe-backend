#include "controller/ticket/TicketController.h"
#include <pqxx/pqxx>
#include "utils/response.h"
#include "utils/Config.h"
#include "utils/uuid.h"
#include "utils/response.h"
#include "utils/Singleton.h"
#include "service/UserService.h"

TicketController::TicketController()
{
    auto &app = getApp();
    CROW_ROUTE(app, "/api/ticket-types").methods("POST"_method)([this](const request &req)
                                                                { return this->createTicketType(req); });

    CROW_ROUTE(app, "/api/ticket-types/<string>").methods("POST"_method)([this](const request &req, const string &id)
                                                                         { return this->updateTicketType(req, id); });
    CROW_ROUTE(app, "/api/ticket-types/<string>").methods("DELETE"_method)([this](const request &req, const string &id)
                                                                           { return this->deleteTicketType(req, id); });
    REST_HANDLER(TicketController, "/my", "GET", getMyTicket);

    CROW_ROUTE(app, "/api/tickets/<string>/refund").methods("POST"_method)([this](const request &req, const string &id)
                                                                           { return this->refund(req, id); });


    // REST_HANDLER(TicketController, "", "POST", createTicket);
    // REST_HANDLER(TicketController, "/update", "POST", updateTicket);
    // REST_HANDLER(TicketController, "", "GET", searchTicket);
    // REST_HANDLER(TicketController, "", "DELETE", deleteTicket);
}

response TicketController::createTicketType(const request &req)
{
    auto body = json::load(req.body);
    
    // auto tticket_type_id = body["ticket_type_id"];
    auto ticket_type_id = generateUUID();
    auto event_id = string(body["event_id"]);
    auto name = string(body["name"]);
    auto description = string(body["description"]);
    auto price = string(body["price"]);
    auto available_quantity = string(body["available_quantity"]);

    pqxx::connection conn{Config::get("database")};

    pqxx::work w{conn};

    string query = R"(
        INSERT INTO "tickettypes" (
    "ticket_type_id",
    "event_id",
    "name",
    "description",
    "price",
    "available_quantity"
    ) VALUES ($1, $2, $3, $4, $5, $6)
    RETURNING *;
    )";

    auto r = w.exec_params(query, ticket_type_id, event_id, name, description, price, available_quantity);

    auto data = resultsToJSON(r);

    w.commit();

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response TicketController::updateTicketType(const request &req, const string &id)
{
    if (id.empty())
    {
        return json::wvalue{{"error", "no id"}};
    }

    auto body = json::load(req.body);
    auto ticket_type_id = id;
    auto event_id = string(body["event_id"]);
    auto name = string(body["name"]);
    auto description = string(body["description"]);
    auto price = string(body["price"]);
    auto available_quantity = string(body["available_quantity"]);

    pqxx::connection conn{Config::get("database")};

    pqxx::work w{conn};

    string query = R"(
        UPDATE "tickettypes"
        SET
        "event_id" = $2,
        "name" = $3,
        "description" = $4,
        "price" = $5,
        "available_quantity" = $6
        WHERE "ticket_type_id" = $1
        RETURNING *;
    )";

    auto r = w.exec_params(query, ticket_type_id, event_id, name, description, price, available_quantity);

    auto data = resultsToJSON(r);

    w.commit();

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response TicketController::deleteTicketType(const request &req, const string &id)
{
    if (id.empty())
    {
        return json::wvalue{{"error", "no id"}};
    }

    auto body = json::load(req.body);
    auto ticket_type_id = id;
    auto event_id = string(body["event_id"]);
    auto name = string(body["name"]);
    auto description = string(body["description"]);
    auto price = string(body["price"]);
    auto available_quantity = string(body["available_quantity"]);

    pqxx::connection conn{Config::get("database")};

    pqxx::work w{conn};

    string query = R"(
        DELETE FROM "tickettypes"
            WHERE "ticket_type_id" = $1
            RETURNING *;
    )";

    auto r = w.exec_params(query, ticket_type_id);

    auto data = resultsToJSON(r);

    w.commit();

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response TicketController::createTicket(const request &req)
{
    CROW_LOG_INFO << req.body;

    auto body = json::load(req.body);

    if (!body || !body.has("userId") || !body.has("eventId") || !body.has("price"))
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

response TicketController::refund(const request &req, const string &id)
{
    if (id.empty())
    {
        return json::wvalue{{"error", "no id"}};
    }
    pqxx::connection conn(Config::get("database"));

    pqxx::work w{conn};

    std::string query = R"(
        WITH deleted_ticket AS (
                    DELETE FROM "tickets"
                    WHERE "ticket_id" = $1
                    RETURNING "order_id"
                )
                UPDATE "orders"
                SET status = 'CANCELLED'
                WHERE "order_id" = (SELECT "order_id" FROM deleted_ticket)
                RETURNING *;
    )";

    auto r = w.exec_params(query, id);

    if (r.empty())
    {
        return json::wvalue{{"error", "Ticket not found or already deleted"}};
    }

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response TicketController::getMyTicket(const request &req)
{
    auto cookieHeader = req.get_header_value("Cookie");
    std::string jwt;

    std::string tokenPrefix = "jwt=";
    auto pos = cookieHeader.find(tokenPrefix);
    if (pos != std::string::npos)
    {
        auto end = cookieHeader.find(';', pos);
        jwt = cookieHeader.substr(pos + tokenPrefix.size(), end - pos - tokenPrefix.size());
    }

    auto decoded = UserService::decodeJWT(jwt);

    CROW_LOG_INFO << decoded.get_header().c_str();

    CROW_LOG_INFO << decoded.get_payload().c_str();

    string id = decoded.get_payload_claim("id").as_string();

    pqxx::connection conn(Config::get("database"));

    pqxx::work w{conn};

    std::string query = R"(
        SELECT t.*
        FROM "tickets" t
        JOIN "orders" o ON t."order_id" = o."order_id"
        WHERE o."user_id" = $1;
    )";

    auto r = w.exec_params(query, id);

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response TicketController::getTicketById(const request &req, const string &id)
{
    if (id.empty())
    {
        return json::wvalue{{"error", "no id"}};
    }

    pqxx::connection conn(Config::get("database"));

    pqxx::work w{conn};

    std::string query = R"(
        select * from tickets where "ticket_id" = $1
        returning *;
    )";

    auto r = w.exec_params(query, id);

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response TicketController::updateTicket(const request &req)
{
    CROW_LOG_INFO << req.body;

    auto body = json::load(req.body);

    if (!body || !body.has("userId") || !body.has("eventId") || !body.has("price") || !body.has("id"))
    {
        return json::wvalue{{"error", "please summit all fields"}};
    }

    for (const auto &key : body.keys())
    {
        if (body[key] == "")
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

    auto r = w.exec_params(query, userId, eventId, price, id);

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
    if (!body || !body.has("id"))
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