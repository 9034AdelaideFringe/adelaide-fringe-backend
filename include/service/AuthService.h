#pragma once

#include "utils/PostgresConnection.h"
#include <string>
#include "crow.h"

using namespace crow;
class AuthService
{
public:
    static bool authJWT(const request& req);
};
