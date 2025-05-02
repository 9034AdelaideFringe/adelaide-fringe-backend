#include "controller/passwordreset/PasswordResetController.h"
#include "utils/Singleton.h"

PasswordResetController::PasswordResetController() : bp_("passwordreset")
{
    // bp_.new_rule_dynamic("/").methods("POST"_method)([this](const request& req){return this->resetPassword(req);});




    getApp().register_blueprint(bp_);
}

void PasswordResetController::resetPassword(const request& req)
{

}