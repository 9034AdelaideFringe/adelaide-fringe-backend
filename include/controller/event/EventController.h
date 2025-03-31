#pragma once
#include "controller/Controller.h"
#include "crow.h"


using namespace crow;
class EventController
{
public:
    REST_CONTROLLER("/event")

    EventController();

    response createEvent(const request& req);
    response updateEvent(const request& req);
    response searchEvent(const request& req);
    response deleteEvent(const request& req);
};
