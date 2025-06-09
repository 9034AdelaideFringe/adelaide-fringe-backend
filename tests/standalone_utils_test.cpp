#include <gtest/gtest.h>
#include "utils/Hash.h"
#include "utils/Config.h"
#include <fstream>

// Hash测试 - 从现有的HashTest.cpp复制
TEST(StandaloneHashTest, GeneratesSHA256Hash) {
    std::string input = "password123";
    std::string expected = "ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f";
    
    std::string actual = sha256(input);
    EXPECT_EQ(actual, expected);
}

TEST(StandaloneHashTest, EmptyStringHash) {
    std::string input = "";
    std::string expected = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
    
    std::string actual = sha256(input);
    EXPECT_EQ(actual, expected);
}

TEST(StandaloneHashTest, ConsistentHashing) {
    std::string input = "test_password";
    
    // 多次哈希应该产生相同的结果
    std::string hash1 = sha256(input);
    std::string hash2 = sha256(input);
    
    EXPECT_EQ(hash1, hash2);
}

// Config测试 - 从现有的ConfigTest.cpp复制
class StandaloneConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试配置文件
        std::ofstream testConfig("standalone_test_config.json");
        testConfig << R"({
            "testKey": "testValue",
            "database": "test_db_connection_string",
            "JWTSecret": "test_jwt_secret"
        })";
        testConfig.close();
    }

    void TearDown() override {
        // 删除测试配置文件
        std::remove("standalone_test_config.json");
    }
};

TEST_F(StandaloneConfigTest, LoadConfig) {
    Config::loadConfig("standalone_test_config.json");
    EXPECT_TRUE(Config::has("testKey"));
    EXPECT_TRUE(Config::has("database"));
    EXPECT_TRUE(Config::has("JWTSecret"));
}

TEST_F(StandaloneConfigTest, GetValue) {
    Config::loadConfig("standalone_test_config.json");
    EXPECT_EQ(Config::get("testKey"), "testValue");
    EXPECT_EQ(Config::get("database"), "test_db_connection_string");
    EXPECT_EQ(Config::get("JWTSecret"), "test_jwt_secret");
}

TEST_F(StandaloneConfigTest, NonExistentKey) {
    Config::loadConfig("standalone_test_config.json");
    EXPECT_FALSE(Config::has("nonExistentKey"));
    EXPECT_EQ(Config::get("nonExistentKey"), "");
}

// 简单的字符串处理测试
TEST(StandaloneUtilsTest, BasicStringOperations) {
    std::string test = "Hello World";
    EXPECT_EQ(test.length(), 11);
    EXPECT_TRUE(test.find("World") != std::string::npos);
}

TEST(StandaloneUtilsTest, NumberValidation) {
    // 测试数字字符串验证
    std::string validNumber = "12345";
    std::string invalidNumber = "abc123";
    
    bool isValid1 = std::all_of(validNumber.begin(), validNumber.end(), ::isdigit);
    bool isValid2 = std::all_of(invalidNumber.begin(), invalidNumber.end(), ::isdigit);
    
    EXPECT_TRUE(isValid1);
    EXPECT_FALSE(isValid2);
} 