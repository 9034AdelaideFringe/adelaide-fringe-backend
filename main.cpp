#include "crow.h"
#include "utils/Singleton.h"
#include "controller/login/LoginController.h"
#include "controller/signup/SignupController.h"
#include "controller/signout/SignoutController.h"
#include "controller/event/EventController.h"
#include "controller/ticket/TicketController.h"
#include "controller/user/UserController.h"
#include "utils/Config.h"
#include "jwt-cpp/jwt.h"
#include "exception/ExceptionHandler.h"
#include "controller/ticket_type/TicketTypeController.h"
#include "controller/cart/CartController.h"

using namespace std;
using namespace crow;

int main()
{
    Config::loadConfig("config.json");

    Blueprint& api = getBlueprint();
    getApp().register_blueprint(api);

    LoginController loginController;
    UserController userController;
    EventController eventController;
    SignupController signupController;
    SignoutController signoutController;
    TicketTypeController ticketTypeController;
    TicketController ticketController;
    
    // CartController cartController;
    
    getApp().server_name("adelaide fringe server");
    getApp().exception_handler(ExceptionHandler::defaultExceptionHandler);

    getApp().port(18080).run();
}