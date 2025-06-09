#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "utils/JWT.h"
#include "utils/Config.h"
#include "jwt-cpp/jwt.h"
#include <fstream>
#include <thread>
#include <chrono>

class JWTTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试配置文件
        std::ofstream testConfig("test_config.json");
        testConfig << R"({
            "database": "test_db_connection",
            "JWTSecret": "test_jwt_secret_key_for_unit_testing_12345"
        })";
        testConfig.close();
        Config::loadConfig("test_config.json");
    }

    void TearDown() override {
        // 删除测试配置文件
        std::remove("test_config.json");
    }
};

TEST_F(JWTTest, GenerateJWTToken) {
    // 测试JWT令牌生成
    std::string token = generateJWT();
    
    // 验证令牌不为空
    EXPECT_FALSE(token.empty());
    
    // 验证令牌格式（JWT有三个部分，用.分隔）
    size_t firstDot = token.find('.');
    size_t secondDot = token.find('.', firstDot + 1);
    
    EXPECT_NE(firstDot, std::string::npos);
    EXPECT_NE(secondDot, std::string::npos);
    EXPECT_TRUE(firstDot < secondDot);
}

TEST_F(JWTTest, DecodeValidJWTToken) {
    // 生成一个JWT令牌
    std::string token = generateJWT();
    
    // 解码JWT令牌
    auto decoded = decodeJWT(token);
    
    // 验证解码成功
    EXPECT_FALSE(decoded.get_header().empty());
    EXPECT_FALSE(decoded.get_payload().empty());
}

TEST_F(JWTTest, DecodeInvalidJWTToken) {
    // 测试解码无效的JWT令牌
    std::string invalidToken = "invalid.jwt.token";
    
    // 解码应该抛出异常
    EXPECT_THROW(decodeJWT(invalidToken), std::exception);
}

TEST_F(JWTTest, JWTWithCustomClaims) {
    // 创建包含自定义声明的JWT令牌
    const std::string secret = Config::get("JWTSecret");
    
    auto token = jwt::create()
        .set_type("JWT")
        .set_issuer("test_issuer")
        .set_payload_claim("user_id", jwt::claim(std::string("test_user_123")))
        .set_payload_claim("role", jwt::claim(std::string("admin")))
        .set_payload_claim("email", jwt::claim(std::string("test@example.com")))
        .sign(jwt::algorithm::hs256{secret});
    
    // 解码令牌并验证声明
    auto decoded = decodeJWT(token);
    
    EXPECT_EQ(decoded.get_payload_claim("user_id").as_string(), "test_user_123");
    EXPECT_EQ(decoded.get_payload_claim("role").as_string(), "admin");
    EXPECT_EQ(decoded.get_payload_claim("email").as_string(), "test@example.com");
    EXPECT_EQ(decoded.get_issuer(), "test_issuer");
}

TEST_F(JWTTest, JWTSignatureVerification) {
    // 使用正确的密钥生成令牌
    const std::string correctSecret = Config::get("JWTSecret");
    
    auto token = jwt::create()
        .set_type("JWT")
        .set_issuer("test_issuer")
        .set_payload_claim("user_id", jwt::claim(std::string("test_user")))
        .sign(jwt::algorithm::hs256{correctSecret});
    
    // 使用正确的密钥验证令牌
    EXPECT_NO_THROW({
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{correctSecret})
            .with_issuer("test_issuer");
        
        auto decoded = jwt::decode(token);
        verifier.verify(decoded);
    });
    
    // 使用错误的密钥验证令牌应该失败
    const std::string wrongSecret = "wrong_secret_key";
    EXPECT_THROW({
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{wrongSecret})
            .with_issuer("test_issuer");
        
        auto decoded = jwt::decode(token);
        verifier.verify(decoded);
    }, std::exception);
}

TEST_F(JWTTest, JWTExpirationHandling) {
    // 创建一个已过期的JWT令牌
    const std::string secret = Config::get("JWTSecret");
    
    // 设置过期时间为1秒前
    auto now = std::chrono::system_clock::now();
    auto expired_time = now - std::chrono::seconds(1);
    
    auto token = jwt::create()
        .set_type("JWT")
        .set_issuer("test_issuer")
        .set_expires_at(expired_time)
        .set_payload_claim("user_id", jwt::claim(std::string("test_user")))
        .sign(jwt::algorithm::hs256{secret});
    
    // 验证过期令牌应该失败
    EXPECT_THROW({
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{secret})
            .with_issuer("test_issuer");
        
        auto decoded = jwt::decode(token);
        verifier.verify(decoded);
    }, std::exception);
}

TEST_F(JWTTest, JWTWithFutureeExpiration) {
    // 创建一个未来过期的JWT令牌
    const std::string secret = Config::get("JWTSecret");
    
    // 设置过期时间为1小时后
    auto now = std::chrono::system_clock::now();
    auto future_time = now + std::chrono::hours(1);
    
    auto token = jwt::create()
        .set_type("JWT")
        .set_issuer("test_issuer")
        .set_expires_at(future_time)
        .set_payload_claim("user_id", jwt::claim(std::string("test_user")))
        .sign(jwt::algorithm::hs256{secret});
    
    // 验证未过期令牌应该成功
    EXPECT_NO_THROW({
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{secret})
            .with_issuer("test_issuer");
        
        auto decoded = jwt::decode(token);
        verifier.verify(decoded);
    });
}

TEST_F(JWTTest, SetCookieFunction) {
    // 测试setCookie函数
    crow::response res;
    std::string testJWT = "test.jwt.token";
    
    setCookie(res, testJWT);
    
    // 验证Cookie是否正确设置
    EXPECT_TRUE(res.headers.count("Set-Cookie") > 0);
    
    std::string cookieValue = res.headers.find("Set-Cookie")->second;
    EXPECT_TRUE(cookieValue.find("jwt=" + testJWT) != std::string::npos);
    EXPECT_TRUE(cookieValue.find("HttpOnly") != std::string::npos);
    EXPECT_TRUE(cookieValue.find("Path=/") != std::string::npos);
}

TEST_F(JWTTest, JWTTokenUniqueness) {
    // 验证每次生成的JWT令牌都是唯一的
    std::string token1 = generateJWT();
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 确保时间戳不同
    std::string token2 = generateJWT();
    
    EXPECT_NE(token1, token2);
}

TEST_F(JWTTest, JWTHeaderValidation) {
    // 测试JWT头部信息
    const std::string secret = Config::get("JWTSecret");
    
    auto token = jwt::create()
        .set_type("JWT")
        .set_algorithm("HS256")
        .set_payload_claim("test", jwt::claim(std::string("value")))
        .sign(jwt::algorithm::hs256{secret});
    
    auto decoded = jwt::decode(token);
    
    // 验证头部信息
    EXPECT_EQ(decoded.get_algorithm(), "HS256");
    EXPECT_EQ(decoded.get_type(), "JWT");
}

TEST_F(JWTTest, EmptySecretHandling) {
    // 测试空密钥的处理
    // 注意：这可能需要修改Config来支持空值测试
    
    // 临时创建一个空密钥的配置
    std::ofstream emptyConfig("empty_config.json");
    emptyConfig << R"({
        "database": "test_db",
        "JWTSecret": ""
    })";
    emptyConfig.close();
    
    Config::loadConfig("empty_config.json");
    
    // 使用空密钥生成JWT应该仍然工作，但不安全
    std::string token = generateJWT();
    EXPECT_FALSE(token.empty());
    
    // 清理
    std::remove("empty_config.json");
    Config::loadConfig("test_config.json"); // 恢复测试配置
} 