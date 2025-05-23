#pragma once

#include "crow.h"
#include "service/AuthService.h"
#include <string>

using namespace std;
using namespace crow;

struct AuthMiddleware : ILocalMiddleware
{
    struct context
    {
        string id;
    };

    void before_handle(crow::request &req, crow::response &res, context &ctx)
    {
        try
        {
            ctx.id = AuthService::authJWT(req);
        }
        catch (const std::exception &e)
        {
            res.code = 401;
            res.write(json::wvalue{{"error", e.what()}}.dump());
            res.end();
        }
    }

    void after_handle(crow::request &req, crow::response &res, context &ctx)
    {
    }
};