#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "controller/login/LoginController.h"
#include "utils/Singleton.h"
#include "service/LoginService.h"
#include "utils/JWT.h"
#include "utils/Hash.h"

// 自定义匹配器来检查Response内容
MATCHER_P(ResponseContains, text, "") {
    return arg.body.find(text) != std::string::npos;
}

MATCHER_P(StatusCodeIs, code, "") {
    return arg.code == code;
}

// 创建一个测试友好的LoginController子类以便访问protected方法
class TestableLoginController : public LoginController {
public:
    using LoginController::LoginController;
    
    // 使用继承得到的login方法
    crow::response testLogin(const crow::request& req) {
        return login(req);
    }
};

class LoginControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在测试开始前设置必要的状态
    }

    void TearDown() override {
        // 清理测试环境
    }

    TestableLoginController controller;
};

TEST_F(LoginControllerTest, LoginWithValidCredentials) {
    // 创建一个包含有效登录凭据的请求
    crow::request req;
    req.body = R"({
        "email": "test@example.com",
        "password": "password123"
    })";
    
    // 模拟用户存在于数据库中
    // 注意：这需要修改实际代码以支持依赖注入或使用全局mock
    
    // 调用login方法
    crow::response res = controller.testLogin(req);
    
    // 验证返回的响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_TRUE(res.headers.count("Set-Cookie") > 0);
    
    // 检查返回的cookie是否包含jwt
    std::string cookie = res.headers.find("Set-Cookie")->second;
    EXPECT_THAT(cookie, ::testing::HasSubstr("jwt="));
}

TEST_F(LoginControllerTest, LoginWithInvalidJSON) {
    // 创建一个包含无效JSON的请求
    crow::request req;
    req.body = "invalid json";
    
    // 调用login方法
    crow::response res = controller.testLogin(req);
    
    // 验证返回响应为错误
    EXPECT_THAT(res, StatusCodeIs(400));
}

TEST_F(LoginControllerTest, LoginWithMissingFields) {
    // 创建一个缺少必填字段的请求
    crow::request req;
    req.body = R"({
        "email": "test@example.com"
        // 缺少password
    })";
    
    // 调用login方法
    crow::response res = controller.testLogin(req);
    
    // 验证返回的响应
    EXPECT_THAT(res, StatusCodeIs(400));
    EXPECT_THAT(res, ResponseContains("missing email or password"));
} 