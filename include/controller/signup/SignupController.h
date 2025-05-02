#pragma once

#include "crow.h"
#include "controller/Controller.h"
#include "utils/PostgresConnection.h"
#include "utils/Config.h"
#include "utils/Hash.h"
#include "utils/Singleton.h"
#include <uuid/uuid.h>

using namespace crow;

class SignupController
{
public:
    REST_CONTROLLER("/api/signup")

    SignupController();
    

private:
    response signup(const request& req);


};