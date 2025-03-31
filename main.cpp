#include "crow.h"
#include "utils/Singleton.h"
#include "controller/login/LoginController.h"
#include "controller/signup/SignupController.h"
#include "controller/signout/SignoutController.h"
#include "controller/event/EventController.h"
#include "controller/ticket/TicketController.h"
#include "utils/Config.h"

using namespace std;
using namespace crow;

int main()
{
    Config::loadConfig("config.json");
    

    LoginController loginController;
    SignupController signupController;
    SignoutController signoutController;
    EventController eventController;
    TicketController ticketController;
    
    getApp().server_name("adelaide fringe server");

    getApp().port(18080).run();


}