#pragma once
#include "controller/Controller.h"
#include "crow.h"

using namespace crow;
using namespace std;

class TicketController
{
public:
    REST_CONTROLLER("/api/tickets")
    TicketController();

    // /api/ticket-types
    response createTicketType(const request& req);

    // /api/ticket-types/:id
    response updateTicketType(const request& req, const string& id);

    // /api/ticket-types/:id
    response deleteTicketType(const request& req, const string& id);

    // /api/tickets/my
    response getMyTicket(const request& req);

    // /api/tickets/:id
    response getTicketById(const request& req, const string& id);
    

//     - POST /api/tickets/:id/refund - 申请退票
// - POST /api/tickets/:id/scan - 扫描票据 (管理员)
    
    // /api/tickets/:id/refund
    response refund(const request& req, const string& id);

    response createTicket(const request& req);
    response updateTicket(const request& req);
    response searchTicket(const request& req);
    response deleteTicket(const request& req);
};
