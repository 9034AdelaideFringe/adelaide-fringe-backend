#include "controller/signout/SignoutController.h"
#include "service/AuthService.h"
#include "utils/response.h"
#include "utils/Singleton.h"

SignoutController::SignoutController() : bp_("signout")
{
    getBlueprint().register_blueprint(bp_);
    CROW_BP_ROUTE(bp_, "").methods("GET"_method)([this](const request& req){
        return this->signout(req);
    });
}

response SignoutController::signout(const request &req)
{
    auto headers = req.headers;
    auto cookie = req.get_header_value("Cookie");
    
    for(const auto& p : req.headers)
    {
        CROW_LOG_INFO << p.first << ": " << p.second;
    }

    CROW_LOG_INFO << cookie;

    response res = json::wvalue{{"message", "ok"}};

    // log the user out!
    res.set_header("Set-Cookie", "jwt=; Expires=Thu, 01 Jan 1970 00:00:00 GMT; HttpOnly; Secure=false; Path=/; SameSite=Strict");


    return res;
}
