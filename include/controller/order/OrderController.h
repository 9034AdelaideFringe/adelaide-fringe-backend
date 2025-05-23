#pragma once

#include "crow.h"
#include <string>

using namespace crow;
using namespace std;

class OrderController
{
public:

    OrderController();

    response getAllOrders(const request& req);
    response getOrderById(const request& req, const string& id);
    response updateOrder(const request& req);
    response deleteOrder(const request& req);
    


private:

    Blueprint bp_;
};
