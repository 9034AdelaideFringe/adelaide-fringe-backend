#pragma once

#include "crow.h"
#include <string>

using namespace crow;
using namespace std;

class TicketTypeController
{
public:
    TicketTypeController();


    response getTicketTypes(const request& req);
    response getTicketTypeByUserId(const request& req, const string& id);
    response getTicketTypeByTicketTypeId(const request& req, const string& id);

private:
    Blueprint bp_;
};