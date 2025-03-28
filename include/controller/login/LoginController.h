#pragma once

#include "crow.h"
#include "utils/Singleton.h"
#include "controller/Controller.h"
#include "utils/Hash.h"
#include "jwt-cpp/jwt.h"
#include "utils/PostgresConnection.h"
#include "utils/Config.h"
#include "utils/JWT.h"
#include <string>

using namespace crow;

class LoginController
{
public:
    REST_CONTROLLER("/login")

    LoginController();

private:
    response login(const request &req);

    response jwt(const request& req);
};
