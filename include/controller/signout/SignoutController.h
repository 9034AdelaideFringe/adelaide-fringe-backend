#pragma once
#include "controller/Controller.h"


using namespace crow;
class SignoutController
{
public:
    REST_CONTROLLER("/signout")

    SignoutController();

    response signout(const request& req);
private:
    Blueprint bp_;
};
