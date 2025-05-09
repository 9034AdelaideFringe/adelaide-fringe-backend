#include <gtest/gtest.h>
#include "utils/Config.h"
#include <fstream>

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试配置文件
        std::ofstream testConfig("test_config.json");
        testConfig << R"({
            "testKey": "testValue",
            "database": "test_db_connection_string",
            "JWTSecret": "test_jwt_secret"
        })";
        testConfig.close();
    }

    void TearDown() override {
        // 删除测试配置文件
        std::remove("test_config.json");
    }
};

TEST_F(ConfigTest, LoadConfig) {
    Config::loadConfig("test_config.json");
    EXPECT_TRUE(Config::has("testKey"));
    EXPECT_TRUE(Config::has("database"));
    EXPECT_TRUE(Config::has("JWTSecret"));
}

TEST_F(ConfigTest, GetValue) {
    Config::loadConfig("test_config.json");
    EXPECT_EQ(Config::get("testKey"), "testValue");
    EXPECT_EQ(Config::get("database"), "test_db_connection_string");
    EXPECT_EQ(Config::get("JWTSecret"), "test_jwt_secret");
}

TEST_F(ConfigTest, NonExistentKey) {
    Config::loadConfig("test_config.json");
    EXPECT_FALSE(Config::has("nonExistentKey"));
    EXPECT_EQ(Config::get("nonExistentKey"), "");
} 