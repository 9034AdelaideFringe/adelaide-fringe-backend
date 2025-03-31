#include "service/EventService.h"
#include "utils/uuid.h"

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