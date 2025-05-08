#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <memory>
#include "utils/Config.h"
#include "utils/PostgresConnection.h"

// 集成测试的基础设置类
class TestEnv : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // 创建测试配置文件
        std::ofstream testConfig("test_config.json");
        testConfig << R"({
            "database": "postgresql://neondb_owner:npg_9nH8RLBWUdro@ep-silent-leaf-a77kylcx-pooler.ap-southeast-2.aws.neon.tech/dev?sslmode=require",
            "JWTSecret": "integration_test_jwt_secret"
        })";
        testConfig.close();
        
        Config::loadConfig("test_config.json");
        
        // 初始化数据库连接
        try {
            conn = std::make_shared<PostgresConnection>(Config::get("database"));
            setupTestDatabase();
        } catch (const std::exception& e) {
            std::cerr << "数据库连接失败: " << e.what() << std::endl;
        }
    }

    static void TearDownTestSuite() {
        cleanupTestDatabase();
        conn.reset();
        std::remove("test_config.json");
    }

    void SetUp() override {
        // 每个测试开始前的设置
    }

    void TearDown() override {
        // 每个测试结束后的清理
    }

    // 设置测试数据库
    static void setupTestDatabase() {
        if (!conn) return;
        
        // 清理任何现有的测试数据
        cleanupTestDatabase();
        
        // 创建测试用户
        conn->execute(R"(
            INSERT INTO "users" ("user_id", "email", "password", "name", "role")
            VALUES 
            ('test-user-1', 'integration-test@example.com', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'Integration Test User', 'user'),
            ('test-admin-1', 'integration-admin@example.com', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'Integration Admin User', 'admin')
            ON CONFLICT DO NOTHING;
        )");
        
        // 创建测试活动
        conn->execute(R"(
            INSERT INTO "events" ("event_id", "title", "description", "location", "start_time", "end_time", "created_by", "capacity")
            VALUES 
            ('test-event-1', '集成测试活动1', '这是用于集成测试的活动1', '测试地点1', '2025-06-01 18:00:00', '2025-06-01 22:00:00', 'test-admin-1', 100),
            ('test-event-2', '集成测试活动2', '这是用于集成测试的活动2', '测试地点2', '2025-06-15 19:00:00', '2025-06-15 23:00:00', 'test-admin-1', 50)
            ON CONFLICT DO NOTHING;
        )");
        
        // 创建测试票类型
        conn->execute(R"(
            INSERT INTO "tickettypes" ("tickettype_id", "event_id", "name", "price", "description", "quantity")
            VALUES 
            ('test-tickettype-1', 'test-event-1', '标准票', 99.99, '标准入场券', 50),
            ('test-tickettype-2', 'test-event-1', 'VIP票', 199.99, 'VIP入场券', 20)
            ON CONFLICT DO NOTHING;
        )");
        
        // 更新活动的票类型关联
        conn->execute(R"(
            UPDATE "events"
            SET "tickettypes" = '[
                {"id": "test-tickettype-1", "name": "标准票", "price": 99.99, "description": "标准入场券", "quantity": 50},
                {"id": "test-tickettype-2", "name": "VIP票", "price": 199.99, "description": "VIP入场券", "quantity": 20}
            ]'::jsonb
            WHERE "event_id" = 'test-event-1';
        )");
    }

    // 清理测试数据库
    static void cleanupTestDatabase() {
        if (!conn) return;
        
        // 根据测试前缀删除测试数据
        conn->execute(R"(DELETE FROM "tickets" WHERE "ticket_id" LIKE 'test-%';)");
        conn->execute(R"(DELETE FROM "tickettypes" WHERE "tickettype_id" LIKE 'test-%';)");
        conn->execute(R"(DELETE FROM "events" WHERE "event_id" LIKE 'test-%';)");
        conn->execute(R"(DELETE FROM "users" WHERE "user_id" LIKE 'test-%';)");
    }

    // 共享数据库连接
    static std::shared_ptr<PostgresConnection> conn;
}; 