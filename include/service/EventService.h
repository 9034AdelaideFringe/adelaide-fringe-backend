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



    static json::wvalue createEvent(const request &req, const string& id = "");

    static json::wvalue updateEvent(const request &req);

    static json::wvalue deleteEvent(const request& req);

private:
    static json::wvalue createEventImpl(const request& req, const string& id);
    
    static json::wvalue updateEventImpl(const request& req);

    static json::wvalue deleteEventImpl(const request& req);


};
