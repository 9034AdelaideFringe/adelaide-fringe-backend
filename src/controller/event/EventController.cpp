#include "controller/event/EventController.h"
#include "service/EventService.h"
#include <pqxx/pqxx>
#include "utils/response.h"
#include "utils/uuid.h"
#include "utils/Config.h"

EventController::EventController()
{
    REST_HANDLER(EventController, "", "POST", createEvent);
    REST_HANDLER(EventController, "/update", "POST", updateEvent);
    REST_HANDLER(EventController, "/search", "POST", searchEvent);
    REST_HANDLER(EventController, "", "DELETE", searchEvent);
    REST_HANDLER(EventController, "", "GET", getEvent);
}

response EventController::createEvent(const request &req)
{
    // EventService::createEvent()
    auto data = json::load(req.body);

    if (!data.has("title") || !data.has("des") || !data.has("location") || !data.has("venue") || !data.has("price") || !data.has("capacity") || !data.has("start") || !data.has("end"))
    {
        return json::wvalue({{"error", "invalid event"}});
    }

    pqxx::connection conn(Config::get("database"));

    std::string id = generateUUID();

    pqxx::work w{conn};
    std::string query = R"(insert into "Events" 
    (title, des, location, "venue", "price", "capacity", "start", "end", "updatedAt", "id") 
    values ($1, $2, $3, $4, $5, $6, $7, $8, NOW(), $9)
    returning *;
    )";
    pqxx::result r = w.exec_params(query,
                                   std::string(data["title"].s()),
                                   std::string(data["des"]),
                                   std::string(data["location"].s()),
                                   std::string(data["venue"].s()),
                                   std::string(data["price"].s()),
                                   std::string(data["capacity"].s()),
                                   std::string(data["start"].s()),
                                   std::string(data["end"].s()),
                                   id);

    json::wvalue event;

    w.commit();

    for (int i = 0; i < r.size(); i++)
    {
        for (int j = 0; j < r.columns(); j++)
        {
            auto item = r.at(i).at(j);
            event[item.name()] = item.as<std::string>();
        }
    }

    CROW_LOG_INFO << event.dump();

    return json::wvalue{{"message", "ok"}, {"data", event}};
}

response EventController::updateEvent(const request &req)
{
    CROW_LOG_INFO << "updateEvent body: " << req.body;
    auto body = json::load(req.body);
    if (!body.has("id"))
    {
        return json::wvalue{{"error", "no id"}};
    }

    for (const auto &key : body.keys())
    {
        if (body[key] == "")
        {
            CROW_LOG_INFO << key << " is empty";
            return json::wvalue{{"error", key + " is empty"}};
        }
    }

    pqxx::connection conn(Config::get("database"));
    pqxx::work w{conn};

    // std::string query = R"(
    //     update "Events" set
    // )";

    // std::vector<std::string> conditions;
    // std::vector<std::string> params;
    // if (body.has("title") && body["title"].s() != "") {
    //     conditions.push_back("title = $" + std::to_string(conditions.size() + 1));
    //     params.push_back(body["title"].s());
    // }

    // if (body.has("des") && body["des"].s() != "") {
    //     conditions.push_back("des = $" + std::to_string(conditions.size() + 1));
    //     params.push_back(body["des"].s());
    // }

    // if (body.has("location") && body["location"].s() != "") {
    //     conditions.push_back("location = $" + std::to_string(conditions.size() + 1));
    //     params.push_back(body["location"].s());
    // }

    // if (body.has("venue") && body["venue"].s() != "") {
    //     conditions.push_back("venue = $" + std::to_string(conditions.size() + 1));
    //     params.push_back(body["venue"].s());
    // }

    // if (body.has("price") && body["price"].s() != "") {
    //     conditions.push_back("price = $" + std::to_string(conditions.size() + 1));
    //     params.push_back(body["price"].s());
    // }

    // if (body.has("capacity") && body["capacity"].s() != "") {
    //     conditions.push_back("capacity = $" + std::to_string(conditions.size() + 1));
    //     params.push_back(body["capacity"].s());
    // }

    // if (body.has("start") && body["start"].s() != "") {
    //     conditions.push_back("start = $" + std::to_string(conditions.size() + 1));
    //     params.push_back(body["start"].s());
    // }

    // if (body.has("end") && body["end"].s() != "") {
    //     conditions.push_back("end = $" + std::to_string(conditions.size() + 1));
    //     params.push_back(body["end"].s());
    // }

    // for(int i = 0; i < conditions.size(); i++)
    // {
    //     std::string cond = conditions[i] + ", ";
    //     if(i == conditions.size() - 1)
    //     {
    //         cond.pop_back();
    //         cond.pop_back();
    //     }
    //     query += cond;
    // }
    // query += " WHERE id = $" + std::to_string(conditions.size() + 1) + " returning *;";
    // params.push_back(body["id"].s());  // 添加 id 参数

    // pqxx::result r = w.exec_params(query, std::apply([](auto&&... args) { return std::make_tuple(args...); }, params));

    std::string query = R"(
        update "Events" set
        title = $1, des = $2, location = $3, venue = $4, price = $5, status = $6, "updatedAt" = NOW(),
        capacity = $7, start = $8, "end" = $9
        where id = $10
        returning *;
    )";

    CROW_LOG_INFO << query;

    pqxx::result r = w.exec_params(query,
                                   std::string(body["title"].s()),
                                   std::string(body["des"].s()),
                                   std::string(body["location"].s()),
                                   std::string(body["venue"].s()),
                                   std::string(body["price"].s()),
                                   std::string(body["status"].s()),
                                   std::string(body["capacity"].s()),
                                   std::string(body["start"].s()),
                                   std::string(body["end"].s()),
                                   std::string(body["id"].s()));

    w.commit();

    json::wvalue event;

    for (int i = 0; i < r.size(); i++)
    {
        for (int j = 0; j < r.columns(); j++)
        {
            std::string name = r[i][j].name();
            event[name] = r[i][j].as<std::string>();
        }
    }

    CROW_LOG_INFO << event.dump();

    return json::wvalue{{"message", "ok"}, {"data", event}};
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


response EventController::deleteEvent(const request &req)
{
    auto body = json::load(req.body);

    if(!body.has("id"))
    {
        return json::wvalue{{"error", "no id"}};
    }

    std::string id = body["id"].s();

    pqxx::connection conn(Config::get("database"));

    pqxx::work w{conn};

    std::string query = R"(
        delete from "Events"
        where id = $1
        returning *;
    )";

    auto r = w.exec_params(query, id);


    w.commit();

    auto data = resultsToJSON(r);
    
    return json::wvalue{{"message", "ok"}, {"data", data}};
}


response EventController::getEvent(const request &req)
{
    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    std::string query = R"(
        select * from "Events";
    )";

    auto r = w.exec(query);

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}