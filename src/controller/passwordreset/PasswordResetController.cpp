#include "controller/passwordreset/PasswordResetController.h"
#include "utils/Singleton.h"
#include "middlewares/AuthMiddleware.h"

PasswordResetController::PasswordResetController() : bp_("passwordreset")
{
    // bp_.new_rule_dynamic("/").methods("POST"_method)([this](const request& req){return this->resetPassword(req);});




    getApp().register_blueprint(bp_);
}

response PasswordResetController::sendEmail(const request &req)
{
    // App<CORSHandler, AuthMiddleware>& app = getApp();
    // auto& ctx = app.get_context<AuthMiddleware>(req);
    // const string& id = ctx.id;

    CROW_LOG_INFO << req.body;

    auto body = json::load(req.body);

    string email(body["email"].s());



    return response();
}

void PasswordResetController::resetPassword(const request &req)
{

}