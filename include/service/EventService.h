#pragma once

#include "utils/PostgresConnection.h"
#include <string>
#include "crow.h"
#include "pqxx/pqxx"

using namespace crow;
using namespace std;

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

    static json::wvalue createEvent(const request &req, const string& id = "");
    static json::wvalue updateEvent(const request &req, const string& id = "");
    static json::wvalue deleteEvent(const request& req, const string& id);

private:
    static json::wvalue createEventImpl(const request& req, const string& id);
    static json::wvalue updateEventImpl(const request& req, const string& id);

    static json::wvalue deleteEventImpl(const request& req, const string& id);
};
