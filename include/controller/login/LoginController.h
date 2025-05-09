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
    // REST_CONTROLLER("/api/login")

    LoginController();

protected:
    response login(const request &req);

private:
    response jwt(const request& req);

    Blueprint bp_;
};
