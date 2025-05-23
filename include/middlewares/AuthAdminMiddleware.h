#pragma once

#include "crow.h"
#include "service/AuthService.h"

using namespace crow;

struct AuthAdminMiddleware : ILocalMiddleware
{
    struct context
    {};

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        AuthService::authAdmin(req);
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {}
};