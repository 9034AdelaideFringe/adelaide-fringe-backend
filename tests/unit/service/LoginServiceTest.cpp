#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "service/LoginService.h"
#include "../utils/MockPostgresConnection.h"
#include "utils/Config.h"
#include <fstream>

class LoginServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试配置文件
        std::ofstream testConfig("test_config.json");
        testConfig << R"({
            "database": "test_db_connection_string",
            "JWTSecret": "test_jwt_secret"
        })";
        testConfig.close();
        
        Config::loadConfig("test_config.json");
    }

    void TearDown() override {
        std::remove("test_config.json");
    }

    MockPostgresConnection mockConn;
};

TEST_F(LoginServiceTest, FindUserSuccess) {
    // 设置MockPostgresConnection的行为
    std::vector<std::vector<std::string>> mockResults = {
        {"user_id", "email", "name", "role"},
        {"user-123", "test@example.com", "Test User", "user"}
    };
    
    EXPECT_CALL(mockConn, execute(::testing::_, ::testing::_))
        .WillOnce(::testing::DoAll(
            ::testing::SetArgReferee<1>(mockResults),
            ::testing::Return(true)
        ));
    
    // 调用被测试的方法
    auto result = LoginService::findUser(mockConn, "test@example.com", "hashedpassword");
    
    // 验证结果
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result[1][0], "user-123");
    ASSERT_EQ(result[1][1], "test@example.com");
}

TEST_F(LoginServiceTest, FindUserNotFound) {
    // 设置MockPostgresConnection返回空结果
    std::vector<std::vector<std::string>> mockResults = {
        {"user_id", "email", "name", "role"}
        // 没有数据行
    };
    
    EXPECT_CALL(mockConn, execute(::testing::_, ::testing::_))
        .WillOnce(::testing::DoAll(
            ::testing::SetArgReferee<1>(mockResults),
            ::testing::Return(true)
        ));
    
    // 调用被测试的方法
    auto result = LoginService::findUser(mockConn, "nonexistent@example.com", "hashedpassword");
    
    // 验证结果
    ASSERT_EQ(result.size(), 1); // 只有表头，没有数据
}

TEST_F(LoginServiceTest, GenerateJWTForUser) {
    // 创建模拟用户数据
    crow::json::wvalue user;
    user["user_id"] = "user-123";
    user["email"] = "test@example.com";
    user["role"] = "user";
    
    // 生成JWT
    std::string jwt = LoginService::generateJWTForUser(user);
    
    // 验证JWT不为空并且格式正确（标准JWT格式是三部分，用点号分隔）
    ASSERT_FALSE(jwt.empty());
    ASSERT_EQ(std::count(jwt.begin(), jwt.end(), '.'), 2);
}

TEST_F(LoginServiceTest, UpdateLastLogin) {
    EXPECT_CALL(mockConn, execute(::testing::_))
        .WillOnce(::testing::Return(true));
    
    bool result = LoginService::updateLastLogin(mockConn, "user-123");
    
    ASSERT_TRUE(result);
} 