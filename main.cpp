#include "crow.h"
#include "utils/Singleton.h"
#include "controller/login/LoginController.h"
#include "controller/signup/SignupController.h"
#include "controller/signout/SignoutController.h"
#include "controller/event/EventController.h"
#include "controller/ticket/TicketController.h"
#include "utils/Config.h"
#include "jwt-cpp/jwt.h"
#include "utils/JWT.h"

using namespace std;
using namespace crow;

int main()
{
    Config::loadConfig("config.json");
    
    // 添加一个简单的健康检查API
    CROW_ROUTE(getApp(), "/health")
    ([]() {
        return crow::response(200, "{\"status\":\"ok\",\"message\":\"Server is running\"}");
    });

    // 添加一个测试登录API，不依赖数据库
    CROW_ROUTE(getApp(), "/api/test-login").methods("POST"_method)
    ([](const request& req) {
        auto body = json::load(req.body);
        if(!body || !body.has("email") || !body.has("password")) {
            return response(status::BAD_REQUEST, "{\"error\":\"missing email or password\"}");
        }
        
        // 创建一个测试用户JSON
        json::wvalue user;
        user["user_id"] = "test-user-id";
        user["email"] = body["email"].s();
        user["name"] = "Test User";
        user["role"] = "user";
        
        // 生成JWT令牌
        auto token = jwt::create()
                        .set_type("JWS")
                        .set_issuer("clx")
                        .set_payload_claim("role", jwt::claim(std::string("user")))
                        .set_payload_claim("user_id", jwt::claim(std::string("test-user-id")))
                        .sign(jwt::algorithm::hs256{Config::get("JWTSecret")});
        
        // 创建响应
        auto res = response(user);
        
        // 设置JWT Cookie
        res.set_header("Set-Cookie", "jwt=" + token + "; HttpOnly; Path=/; SameSite=Strict");
        
        return res;
    });

    LoginController loginController;
    SignupController signupController;
    SignoutController signoutController;
    EventController eventController;
    TicketController ticketController;
    
    getApp().server_name("adelaide fringe server");

    getApp().port(18080).run();
}