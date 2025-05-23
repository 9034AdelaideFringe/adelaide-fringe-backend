#pragma once
#include "crow.h"

using namespace crow;
class PasswordResetController
{
public:

    PasswordResetController(/* args */);

    response sendEmail(const request& req);

    void resetPassword(const request& req);

private:
    Blueprint bp_;
};
