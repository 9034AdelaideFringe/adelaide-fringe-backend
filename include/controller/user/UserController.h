#pragma once

#include "crow.h"


using namespace crow;

class UserController
{
public:
    UserController();

    response getUser(const request& req);
    response updateUser(const request& req);

private: 
    Blueprint bp_;
};
