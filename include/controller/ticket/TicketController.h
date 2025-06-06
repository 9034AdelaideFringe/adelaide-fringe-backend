#pragma once
#include "crow.h"
#include <string>

using namespace crow;
using namespace std;

class TicketController
{
public:
    TicketController();

    response getAllTickets(const request& req);
    // response createTicket(const request& req);
    response updateTicket(const request& req);
    response getTicket(const request& req, const string& id);
    response deleteTicket(const request& req);

    response getTicketsByUserId(const request& req, const string& id);

    // scan
    response scanTicket(const request& req, const string& id);

    response getTicketsByEventId(const request& req, const string& id);

private:
    Blueprint bp_;
};
