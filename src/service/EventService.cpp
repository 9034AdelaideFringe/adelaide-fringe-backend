#include "service/EventService.h"
#include "utils/uuid.h"
#include "service/UploadService.h"
#include "utils/Config.h"
#include "utils/response.h"

QueryResult EventService::createEvent(const PostgresConnection &conn,
                                      const std::string &eventName,
                                      const std::string &des,
                                      const std::string &location,
                                      const std::string &venue,
                                      const std::string &price,
                                      const std::string &capacity,
                                      const std::string &start,
                                      const std::string &end)
{
    std::string id = generateUUID();
    // std::string query = R"(insert into "Events" (id, title, des, location, venue, price, "updatedAt", capacity))";
    // query += "values (" + id + ", " + ""

    char buf[1024];
    snprintf(buf, sizeof buf, R"(insert into "Events" (id, title, des, location, venue, price, "updatedAt", capacity) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s'))",
             id.c_str(),
             eventName.c_str(),
             des.c_str(),
             location.c_str(),
             venue.c_str(),
             price.c_str(),
             capacity.c_str(),
             start.c_str(),
             end.c_str());

    std::string query = buf;
    QueryResult result;
    conn.execute(query, result);
    return result;
}

json::wvalue EventService::createEvent(const request &req, const string &id)
{
    return createEventImpl(req, id);
}

json::wvalue EventService::updateEvent(const request &req, const string &id)
{
    return updateEventImpl(req, id);
}

json::wvalue EventService::deleteEvent(const request &req, const string &id)
{
    return deleteEventImpl(req, id);
}

json::wvalue EventService::createEventImpl(const request &req, const string &id)
{
    json::wvalue data;

    multipart::message_view fileMessage(req);

    auto titlePart = fileMessage.get_part_by_name("title").body;
    auto descriptionPart = fileMessage.get_part_by_name("description").body;
    auto short_descriptionPart = fileMessage.get_part_by_name("short_description").body;
    auto imagePart = fileMessage.get_part_by_name("image");
    auto venueSeatingLayoutPart = fileMessage.get_part_by_name("venueSeatingLayout");
    auto datePart = fileMessage.get_part_by_name("date").body;
    auto timePart = fileMessage.get_part_by_name("time").body;
    auto end_timePart = fileMessage.get_part_by_name("end_time").body;
    auto venuePart = fileMessage.get_part_by_name("venue").body;
    auto capacityPart = fileMessage.get_part_by_name("capacity").body;
    auto categoryPart = fileMessage.get_part_by_name("category").body;
    auto statusPart = fileMessage.get_part_by_name("status").body;
    auto created_byPart = fileMessage.get_part_by_name("created_by").body;

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

    pqxx::connection conn(Config::get("database"));

    std::string event_id = (id == "" ? generateUUID() : id);

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

    CROW_LOG_INFO << data.dump();

    for (auto &key : data.keys())
    {
        if (data[key].dump() == R"("")")
        {
            return json::wvalue{{"error", key + " is empty"}};
        }
    }

    w.commit();

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

json::wvalue EventService::updateEventImpl(const request &req, const string &id)
{

    json::wvalue data;

    multipart::message_view fileMessage(req);

    auto titlePart = fileMessage.get_part_by_name("title").body;
    auto descriptionPart = fileMessage.get_part_by_name("description").body;
    auto short_descriptionPart = fileMessage.get_part_by_name("short_description").body;
    auto imagePart = fileMessage.get_part_by_name("image");
    auto venueSeatingLayoutPart = fileMessage.get_part_by_name("venueSeatingLayout");
    auto datePart = fileMessage.get_part_by_name("date").body;
    auto timePart = fileMessage.get_part_by_name("time").body;
    auto end_timePart = fileMessage.get_part_by_name("end_time").body;
    auto venuePart = fileMessage.get_part_by_name("venue").body;
    auto capacityPart = fileMessage.get_part_by_name("capacity").body;
    auto categoryPart = fileMessage.get_part_by_name("category").body;
    auto statusPart = fileMessage.get_part_by_name("status").body;
    auto created_byPart = fileMessage.get_part_by_name("created_by").body;

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

    pqxx::connection conn(Config::get("database"));

    std::string event_id = id;

    pqxx::work w{conn};
    std::string query = R"(update "events" 
    set
    "title" = $2,
    "description" = $3,
    "short_description" = $4,
    "image" = $5,
    "venueseatinglayout" = $6,
    "date" = $7,
    "time" = $8,
    "end_time" = $9,
    "venue" = $10,
    "capacity" = $11,
    "category" = $12,
    "status" = $13,
    "created_by" = $14
    WHERE "event_id" = $1
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

    CROW_LOG_INFO << data.dump();


    w.commit();

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

json::wvalue EventService::deleteEventImpl(const request &req, const string &id)
{
    pqxx::connection conn{Config::get("database")};

    std::string event_id = id;

    pqxx::work w{conn};

    std::string query = R"(delete from "events" 
        WHERE "event_id" = $1
        returning *;
    )";

    pqxx::result r = w.exec_params(query,
                                   event_id
                                   );

    auto data = resultsToJSON(r);

    CROW_LOG_INFO << data.dump();

    w.commit();

    return json::wvalue{{"message", "ok"}, {"data", data}};
}