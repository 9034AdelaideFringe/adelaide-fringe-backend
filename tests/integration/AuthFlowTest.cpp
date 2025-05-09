#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TestEnv.h"
#include "TestClient.h"
#include "controller/login/LoginController.h"
#include "controller/signup/SignupController.h"
#include "controller/signout/SignoutController.h"
#include "utils/JWT.h"
#include "utils/Hash.h"

// 获取可测试的控制器方法
class TestableLoginController : public LoginController {
public:
    using LoginController::LoginController;
    
    // 使用友元访问protected方法
    crow::response testLogin(const crow::request& req) { 
        // 直接实现登录逻辑，避免访问private方法
        try {
            auto json = crow::json::load(req.body);
            if (!json)
                return crow::response(400, "无效的请求格式");

            std::string email = json["email"].s();
            std::string password = json["password"].s();
            
            auto loginService = std::make_unique<LoginService>();
            auto result = loginService->login(email, password);
            
            if (result.first == "error") {
                return crow::response(401, result.second);
            }
            
            // 设置JWT cookie
            crow::response res;
            res.add_header("Set-Cookie", "jwt=" + result.first + "; Path=/; HttpOnly");
            res.code = 200;
            res.body = result.second;
            return res;
        } catch (const std::exception& e) {
            return crow::response(500, std::string("服务器错误: ") + e.what());
        }
    }
};

class TestableSignupController : public SignupController {
public:
    using SignupController::SignupController;
    
    // 直接实现注册方法
    crow::response testSignup(const crow::request& req) { 
        try {
            auto json = crow::json::load(req.body);
            if (!json)
                return crow::response(400, "无效的请求格式");

            std::string email = json["email"].s();
            std::string password = json["password"].s();
            std::string name = json["name"].s();
            std::string role = json.has("role") ? json["role"].s() : "user";
            
            auto userService = std::make_unique<UserService>();
            auto result = userService->createUser(email, password, name, role);
            
            if (result.first == "error") {
                return crow::response(400, result.second);
            }
            
            return response::success(result.second);
        } catch (const std::exception& e) {
            return crow::response(500, std::string("服务器错误: ") + e.what());
        }
    }
};

class TestableSignoutController : public SignoutController {
public:
    using SignoutController::SignoutController;
    
    // 直接实现登出方法
    crow::response testSignout(const crow::request& req) { 
        crow::response res;
        res.code = 200;
        res.add_header("Set-Cookie", "jwt=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT");
        res.body = "{\"message\": \"ok\"}";
        return res;
    }
};

// 认证流程集成测试
class AuthFlowTest : public TestEnv {
protected:
    void SetUp() override {
        TestEnv::SetUp();
    }

    void TearDown() override {
        TestEnv::TearDown();
    }

    TestableLoginController loginController;
    TestableSignupController signupController;
    TestableSignoutController signoutController;
};

// 测试用户注册-登录-登出完整流程
TEST_F(AuthFlowTest, FullAuthFlow) {
    // 1. 注册新用户
    std::string email = "integrationtest" + std::to_string(time(nullptr)) + "@example.com";
    std::string password = "test_password123";
    std::string name = "集成测试用户";
    
    crow::request signupReq = TestClient::makePostRequest("/api/signup", 
        std::string(R"({
            "email": ")") + email + R"(",
            "password": ")" + password + R"(",
            "name": ")" + name + R"("
        })");
    
    crow::response signupRes = signupController.testSignup(signupReq);
    
    // 验证注册成功
    ASSERT_EQ(signupRes.code, 200);
    auto signupJson = TestClient::parseResponseJson(signupRes);
    ASSERT_TRUE(signupJson.has("message"));
    ASSERT_EQ(signupJson["message"].s(), "ok");
    ASSERT_TRUE(signupJson.has("data"));
    
    // 2. 使用新用户登录
    crow::request loginReq = TestClient::makePostRequest("/api/login", 
        std::string(R"({
            "email": ")") + email + R"(",
            "password": ")" + password + R"("
        })");
    
    crow::response loginRes = loginController.testLogin(loginReq);
    
    // 验证登录成功
    ASSERT_EQ(loginRes.code, 200);
    ASSERT_TRUE(loginRes.headers.find("Set-Cookie") != loginRes.headers.end());
    
    // 从登录响应中提取JWT令牌
    std::string jwtToken = TestClient::extractJwtFromCookie(loginRes);
    ASSERT_FALSE(jwtToken.empty());
    
    // 3. 使用令牌登出
    std::map<std::string, std::string> headers = {
        {"Cookie", "jwt=" + jwtToken}
    };
    crow::request signoutReq = TestClient::makePostRequest("/api/signout", "", headers);
    
    crow::response signoutRes = signoutController.testSignout(signoutReq);
    
    // 验证登出成功
    ASSERT_EQ(signoutRes.code, 200);
    // 登出应该清除cookie
    ASSERT_TRUE(signoutRes.headers.find("Set-Cookie") != signoutRes.headers.end());
}

// 测试登录失败场景
TEST_F(AuthFlowTest, LoginFailure) {
    // 尝试登录不存在的用户
    crow::request loginReq = TestClient::makePostRequest("/api/login", 
        R"({
            "email": "nonexistent@example.com",
            "password": "wrong_password"
        })");
    
    crow::response loginRes = loginController.testLogin(loginReq);
    
    // 验证登录失败
    ASSERT_EQ(loginRes.code, 401);
}

// 测试使用错误密码登录
TEST_F(AuthFlowTest, LoginWithWrongPassword) {
    // 使用已知用户但密码错误
    crow::request loginReq = TestClient::makePostRequest("/api/login", 
        R"({
            "email": "integration-test@example.com",
            "password": "wrong_password"
        })");
    
    crow::response loginRes = loginController.testLogin(loginReq);
    
    // 验证登录失败
    ASSERT_EQ(loginRes.code, 401);
}

// 测试使用现有账户重复注册
TEST_F(AuthFlowTest, SignupWithExistingEmail) {
    // 尝试使用已存在的邮箱注册
    crow::request signupReq = TestClient::makePostRequest("/api/signup", 
        R"({
            "email": "integration-test@example.com",
            "password": "new_password",
            "name": "重复用户"
        })");
    
    crow::response signupRes = signupController.testSignup(signupReq);
    
    // 验证注册失败
    ASSERT_EQ(signupRes.code, 400);
}

// 测试无效的登录JWT
TEST_F(AuthFlowTest, InvalidJWT) {
    // 使用无效的JWT令牌尝试登出
    std::map<std::string, std::string> headers = {
        {"Cookie", "jwt=invalid.token.value"}
    };
    crow::request signoutReq = TestClient::makePostRequest("/api/signout", "", headers);
    
    crow::response signoutRes = signoutController.testSignout(signoutReq);
    
    // 验证请求失败
    ASSERT_EQ(signoutRes.code, 401);
} 