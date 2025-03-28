#include "crow.h"
#include "utils/Singleton.h"
#include "controller/login/LoginController.h"
#include "controller/signup/SignupController.h"
#include "utils/Config.h"

using namespace std;
using namespace crow;

int main()
{
    Config::loadConfig("config.json");
    

    LoginController loginController;
    SignupController signupController;
    
    getApp().server_name("adelaide fringe server");

    getApp().port(18080).run();


}