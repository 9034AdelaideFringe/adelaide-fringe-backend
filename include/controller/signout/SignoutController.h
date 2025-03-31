#pragma once
#include "controller/Controller.h"


using namespace crow;
class SignoutController
{
public:
    REST_CONTROLLER("/signout")

    SignoutController(/* args */);

    response signout(const request& req);
};
