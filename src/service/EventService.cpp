#include "service/EventService.h"
#include "utils/uuid.h"
#include "service/UploadService.h"
#include "utils/Config.h"
#include "utils/response.h"
// #include "utils/DB.hpp"

json::wvalue EventService::createEvent(const request &req)
{
    return createEventImpl(req);
}

json::wvalue EventService::updateEvent(const request &req)
{
    return updateEventImpl(req);
}

json::wvalue EventService::deleteEvent(const request &req)
{
    return deleteEventImpl(req);
}

json::wvalue EventService::createEventImpl(const request &req)
{
    json::wvalue data;

    multipart::message_view fileMessage(req);

    auto titlePart = fileMessage.get_part_by_name("title").body;

    auto descriptionPart = fileMessage.get_part_by_name("description").body;

    auto short_descriptionPart = fileMessage.get_part_by_name("short_description").body;

    auto imagePart = fileMessage.get_part_by_name("image");
    auto venueSeatingLayoutPart = fileMessage.get_part_by_name("venueSeatingLayout");
    if(imagePart.body.empty() || venueSeatingLayoutPart.body.empty())
    {
        return json::wvalue{{"error", "missing image or venueSeatingLayout"}};
    }

    auto datePart = fileMessage.get_part_by_name("date").body;

    auto timePart = fileMessage.get_part_by_name("time").body;

    auto end_timePart = fileMessage.get_part_by_name("end_time").body;

    auto venuePart = fileMessage.get_part_by_name("venue").body;

    auto capacityPart = fileMessage.get_part_by_name("capacity").body;

    auto categoryPart = fileMessage.get_part_by_name("category").body;

    auto statusPart = fileMessage.get_part_by_name("status").body;

    auto created_byPart = fileMessage.get_part_by_name("created_by").body;

    auto ticket_typesPart = fileMessage.get_part_by_name("ticket_types").body;

    auto title = std::string(titlePart);
    auto description = std::string(descriptionPart);
    auto short_description = std::string(short_descriptionPart);
    auto image = UploadService::uploadFile(imagePart);
    auto venueSeatingLayout = UploadService::uploadFile(venueSeatingLayoutPart);
    auto date = std::string(datePart);
    auto time = std::string(timePart);
    auto end_time = std::string(end_timePart);
    auto venue = std::string(venuePart);
    auto capacity = std::string(capacityPart);
    auto category = std::string(categoryPart);
    auto status = std::string(statusPart);
    auto created_by = std::string(created_byPart);
    auto ticket_types = string(ticket_typesPart);

    pqxx::connection conn(Config::get("database"));

    std::string event_id = generateUUID();

    pqxx::work w{conn};
    std::string query = R"(insert into "events" 
    (
    "event_id",
    "title", 
    "description", 
    "short_description", 
    "image", 
    "venueseatinglayout", 
    "date", 
    "time", 
    "end_time", 
    "venue", 
    "capacity",
    "category",
    "status",
    "created_by"
    ) 
    values ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14)
    returning *;
    )";

    pqxx::result r = w.exec_params(query,
                                   event_id,
                                   title,
                                   description,
                                   short_description,
                                   image,
                                   venueSeatingLayout,
                                   date,
                                   time,
                                   end_time,
                                   venue,
                                   capacity,
                                   category,
                                   status,
                                   created_by);

    data = resultsToJSON(r);


    query = R"(
        insert into "tickettypes" ("ticket_type_id", "event_id", "name", "description", "price", "available_quantity") values
    )";


    auto ticket_typeData = json::wvalue();


    auto ticket_typesJSONArray = json::load(ticket_types);
    vector<string> params;
    for(int i = 0; i < ticket_typesJSONArray.size(); i++)
    {
        const json::rvalue& ticket_typeJSON = ticket_typesJSONArray[i];
        string ticket_type_id = generateUUID();
        string name = ticket_typeJSON["name"].s();
        string description = ticket_typeJSON["description"].s();
        string price = ticket_typeJSON["price"].s();
        string available_quantity = ticket_typeJSON["available_quantity"].s();

        string values = "($1, $2, $3, $4, $5, $6) ";
        string fullQuery = query + values + "returning *";

        auto r = w.exec_params(fullQuery, ticket_type_id, event_id, name, description, price, available_quantity);
        // auto ticket_typeResults = resultsToJSON(r);

        ticket_typeData[i] = move(resultsToJSON(r)[0]);
        
    }


    CROW_LOG_INFO << data.dump();

    for (auto &key : data.keys())
    {
        if (data[key].dump() == R"("")")
        {
            return json::wvalue{{"error", key + " is empty"}};
        }
    }

    w.commit();

    return json::wvalue{{"message", "ok"}, {"data", data}, {"ticket_types", ticket_typeData}};
}

json::wvalue EventService::updateEventImpl(const request &req)
{
    json::rvalue body = json::load(req.body);

    // multipart::message_view fileMessage(req);

    if (!body ||
        !body.has("event_id") ||
        !body.has("title") ||
        !body.has("description") ||
        !body.has("short_description") ||
        !body.has("date") ||
        !body.has("time") ||
        !body.has("end_time") ||
        !body.has("venue") ||
        !body.has("capacity") ||
        !body.has("category") ||
        !body.has("status") ||
        !body.has("created_by"))
    {
        return json::wvalue{{"error", "missing fields"}};
    }

    // auto event_idPart = fileMessage.get_part_by_name("event_id").body;
    // auto titlePart = fileMessage.get_part_by_name("title").body;
    // auto descriptionPart = fileMessage.get_part_by_name("description").body;
    // auto short_descriptionPart = fileMessage.get_part_by_name("short_description").body;
    // auto datePart = fileMessage.get_part_by_name("date").body;
    // auto timePart = fileMessage.get_part_by_name("time").body;
    // auto end_timePart = fileMessage.get_part_by_name("end_time").body;
    // auto venuePart = fileMessage.get_part_by_name("venue").body;
    // auto capacityPart = fileMessage.get_part_by_name("capacity").body;
    // auto categoryPart = fileMessage.get_part_by_name("category").body;
    // auto statusPart = fileMessage.get_part_by_name("status").body;
    // auto created_byPart = fileMessage.get_part_by_name("created_by").body;

    auto event_id = std::string(body["event_id"].s());
    auto title = std::string(body["title"].s());
    auto description = std::string(body["description"].s());
    auto short_description = std::string(body["short_description"].s());
    auto date = std::string(body["date"].s());
    auto time = std::string(body["time"].s());
    auto end_time = std::string(body["end_time"].s());
    auto venue = std::string(body["venue"].s());
    auto capacity = std::string(body["capacity"].s());
    auto category = std::string(body["category"].s());
    auto status = std::string(body["status"].s());
    auto created_by = std::string(body["created_by"].s());

    pqxx::connection conn(Config::get("database"));

    pqxx::work w{conn};
    std::string query = R"(update "events" 
    set
    "title" = $2,
    "description" = $3,
    "short_description" = $4,
    "date" = $5,
    "time" = $6,
    "end_time" = $7,
    "venue" = $8,
    "capacity" = $9,
    "category" = $10,
    "status" = $11,
    "created_by" = $12
    WHERE "event_id" = $1
    returning *;
    )";

    pqxx::result r = w.exec_params(query,
                                   event_id,
                                   title,
                                   description,
                                   short_description,
                                   date,
                                   time,
                                   end_time,
                                   venue,
                                   capacity,
                                   category,
                                   status,
                                   created_by);

    auto data = resultsToJSON(r);

    CROW_LOG_INFO << data.dump();

    w.commit();

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

json::wvalue EventService::deleteEventImpl(const request &req)
{
    json::rvalue body = json::load(req.body);
    if(!body || !body.has("event_id"))
    {
        return json::wvalue{{"error", "missing id"}};
    }

    pqxx::connection conn{Config::get("database")};

    std::string event_id = body["event_id"].s();

    pqxx::work w{conn};

    std::string query = R"(delete from "events" 
        WHERE "event_id" = $1
        returning *;
    )";

    pqxx::result r = w.exec_params(query,
                                   event_id);

    auto data = resultsToJSON(r);

    CROW_LOG_INFO << data.dump();

    w.commit();

    return json::wvalue{{"message", "ok"}, {"data", data}};
}