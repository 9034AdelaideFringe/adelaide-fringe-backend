#pragma once

#include "utils/PostgresConnection.h"
#include <string>

class EventService
{
public:
    static QueryResult createEvent(const PostgresConnection &conn,
                                   const std::string &eventName,
                                   const std::string &des, 
                                   const std::string &location,
                                   const std::string &venue,
                                   const std::string& price,
                                   const std::string& capacity,
                                   const std::string &start,
                                   const std::string &end);
};
