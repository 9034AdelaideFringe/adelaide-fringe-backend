#pragma once
// #include "controller/Controller.h"
#include "crow.h"


using namespace crow;
class PasswordResetController
{
public:

    PasswordResetController(/* args */);

    void resetPassword(const request& req);

private:
    Blueprint bp_;
};
