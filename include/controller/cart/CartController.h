#pragma once

#include "crow.h"
#include <string>

using namespace crow;
using namespace std;

class CartController
{
public:
    CartController();

    response getCartByUserId(const request& req, const string& id);
    response addToCart(const request& req);
    response updateCart(const request& req);
    response removeFromCart(const request& req);
    response checkOut(const request& req, const string& id);

private:
    Blueprint bp_;
};