#pragma once
#include "controller/Controller.h"
#include "crow.h"

using namespace crow;

class TicketController
{
public:
    REST_CONTROLLER("/ticket")
    TicketController(/* args */);

    response createTicket(const request& req);
    response updateTicket(const request& req);
    response searchTicket(const request& req);
    response deleteTicket(const request& req);
};
