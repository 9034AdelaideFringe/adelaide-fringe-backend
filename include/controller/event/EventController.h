#pragma once
#include "controller/Controller.h"
#include "crow.h"
#include <string>


using namespace crow;
using namespace std;

class EventController
{
public:
    REST_CONTROLLER("/api/events")

    EventController();

    response createEvent(const request& req);
    response updateEvent(const request& req, const string& id);
    response searchEvent(const request& req);
    response deleteEvent(const request& req, const string& id);

    response getEvent(const request& req);
    response getEventById(const string& id);
    response getTicketTypes(const request& req, const string& id);
};
