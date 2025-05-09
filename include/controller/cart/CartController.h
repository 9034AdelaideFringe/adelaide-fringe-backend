#pragma once

#include "crow.h"

using namespace crow;

class CartController
{
public:
    CartController();

    response getCart(const request& req);
    response addToCart(const request& req);
    response updateCart(const request& req);
    response removeFromCart(const request& req);
    // response checkOut(const request& req);

private:
    Blueprint bp_;
};