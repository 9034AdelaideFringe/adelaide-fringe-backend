#include "controller/event/EventController.h"
#include "service/EventService.h"
#include <pqxx/pqxx>
#include "utils/response.h"
#include "utils/uuid.h"
#include "utils/Config.h"
#include "utils/Singleton.h"
#include "service/UploadService.h"
#include "service/EventService.h"

EventController::EventController()
{
    // REST_HANDLER(EventController, "/search", "POST", searchEvent);
    REST_HANDLER(EventController, "", "GET", getEvent);

    CROW_ROUTE(getApp(), "/api/events/<string>").methods("POST"_method)([this](const std::string &id)
                                                                        { return this->getEventById(id); });

    // CROW_ROUTE(getApp(), "/api/events").methods("POST"_method)([this](const request& req, string& id)
    //                                                                     { return this->createEvent(req, id); });

    REST_HANDLER(EventController, "", "POST", createEvent);

    CROW_ROUTE(getApp(), "/api/events/<string>").methods("PUT"_method)([this](const request &req, const std::string &id)
                                                                       { return this->updateEvent(req, id); });

    CROW_ROUTE(getApp(), "/api/events/<string>").methods("DELETE"_method)([this](const request &req, const std::string &id)
                                                                          { return this->deleteEvent(req, id); });

    CROW_ROUTE(getApp(), "/api/events/<string>/ticket-type").methods("GET"_method)([this](const request &req, const string &id)
                                                                                   { return this->getTicketTypes(req, id); });
}

response EventController::createEvent(const request &req)
{
    return EventService::createEvent(req);
}

response EventController::updateEvent(const request &req, const std::string &id)
{
    return EventService::updateEvent(req, id);
}

response EventController::searchEvent(const request &req)
{
    // CROW_LOG_INFO << req.url_params.get("title");
    CROW_LOG_INFO << req.body;
    auto body = json::load(req.body);

    if (!body.has("title"))
    {
        return json::wvalue{{"error", "no title"}};
    }

    std::string title = body["title"].s();

    pqxx::connection conn(Config::get("database"));
    pqxx::work w{conn};

    std::string query = R"(
        select * from "Events"
        where title like $1
    )";

    CROW_LOG_INFO << query;

    auto r = w.exec_params(query, "%" + title + "%");

    w.commit();

    std::vector<json::wvalue> objs;
    for (int i = 0; i < r.size(); i++)
    {
        json::wvalue event;
        for (int j = 0; j < r.columns(); j++)
        {
            std::string name = r[i][j].name();
            // event[name] = "";
            std::string value = r[i][j].as<std::string>();
            event[name] = value;
        }
        objs.push_back(event);
    }
    json::wvalue events = std::move(objs);
    CROW_LOG_INFO << events.dump();

    return json::wvalue{{"message", "ok"}, {"data", events}};
}

response EventController::deleteEvent(const request &req, const string &id)
{

    if (id.empty())
    {
        return json::wvalue{{"error", "no id"}};
    }

    return EventService::deleteEvent(req, id);
}

response EventController::getEvent(const request &req)
{
    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    std::string query = R"(
        select * from "events";
    )";

    auto r = w.exec(query);

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response EventController::getEventById(const std::string &id)
{

    if (id.empty())
    {
        return json::wvalue{{"error", "id is empty"}};
    }

    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    std::string query = R"(
        select * from "events"
        where "event_id" = $1
    )";

    auto r = w.exec(query, id);
    w.commit();

    auto data = resultsToJSON(r);

    CROW_LOG_INFO << data.dump();

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response EventController::getTicketTypes(const request &req, const string &id)
{
    if (id.empty())
    {
        return json::wvalue{{"error", "id is empty"}};
    }

    pqxx::connection conn{Config::get("database")};

    pqxx::work w{conn};

    string query = R"(
        select "tickettypes" from "events"
        where "event_id" = $1
    )";

    auto r = w.exec(query, id);

    w.commit();

    auto data = resultsToJSON(r);
    CROW_LOG_INFO << data.dump();

    return json::wvalue{{"message", "ok"}, {"data", data}};
}