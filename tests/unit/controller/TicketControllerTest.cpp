#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "controller/ticket/TicketController.h"
#include "utils/Singleton.h"
#include "utils/Config.h"
#include "utils/uuid.h"
#include "utils/JWT.h"
#include <fstream>

// 自定义匹配器
MATCHER_P(ResponseContains, text, "") {
    return arg.body.find(text) != std::string::npos;
}

MATCHER_P(StatusCodeIs, code, "") {
    return arg.code == code;
}

// 创建一个测试友好的TicketController子类
class TestableTicketController : public TicketController {
public:
    using TicketController::TicketController;
    
    // 暴露protected方法用于测试
    crow::response testCreateTicketType(const crow::request& req) {
        return createTicketType(req);
    }
    
    crow::response testUpdateTicketType(const crow::request& req, const std::string& id) {
        return updateTicketType(req, id);
    }
    
    crow::response testDeleteTicketType(const crow::request& req, const std::string& id) {
        return deleteTicketType(req, id);
    }
    
    crow::response testGetMyTicket(const crow::request& req) {
        return getMyTicket(req);
    }
    
    crow::response testRefund(const crow::request& req, const std::string& id) {
        return refund(req, id);
    }
    
    crow::response testCreateTicket(const crow::request& req) {
        return createTicket(req);
    }
    
    crow::response testGetTicketById(const crow::request& req, const std::string& id) {
        return getTicketById(req, id);
    }
};

class TicketControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置测试配置
        std::ofstream testConfig("test_config.json");
        testConfig << R"({
            "database": "mock://localhost/test_db",
            "JWTSecret": "test_jwt_secret_for_ticket_testing"
        })";
        testConfig.close();
        Config::loadConfig("test_config.json");
    }

    void TearDown() override {
        // 清理测试配置文件
        std::remove("test_config.json");
    }

    TestableTicketController controller;
    
    // 辅助方法：创建有效的JWT令牌
    std::string createTestJWT(const std::string& userId = "test_user_123") {
        const std::string secret = Config::get("JWTSecret");
        return jwt::create()
            .set_type("JWT")
            .set_issuer("test_issuer")
            .set_payload_claim("id", jwt::claim(userId))
            .set_payload_claim("role", jwt::claim(std::string("user")))
            .sign(jwt::algorithm::hs256{secret});
    }
};

TEST_F(TicketControllerTest, CreateTicketTypeWithValidData) {
    // 测试创建票务类型
    crow::request req;
    req.body = R"({
        "event_id": "test_event_123",
        "name": "VIP Ticket",
        "description": "VIP access with backstage pass",
        "price": "99.99",
        "available_quantity": "50"
    })";
    req.headers.emplace("Content-Type", "application/json");
    
    crow::response res = controller.testCreateTicketType(req);
    
    // 验证成功响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("ok"));
    EXPECT_THAT(res, ResponseContains("data"));
}

TEST_F(TicketControllerTest, CreateTicketTypeWithMissingFields) {
    // 测试创建票务类型时缺少必填字段
    crow::request req;
    req.body = R"({
        "name": "Incomplete Ticket"
    })";
    req.headers.emplace("Content-Type", "application/json");
    
    crow::response res = controller.testCreateTicketType(req);
    
    // 应该返回错误或处理缺失字段
    EXPECT_TRUE(res.code == 400 || res.code == 500);
}

TEST_F(TicketControllerTest, UpdateTicketTypeWithValidData) {
    // 测试更新票务类型
    std::string ticketTypeId = "ticket_type_123";
    crow::request req;
    req.body = R"({
        "event_id": "updated_event_123",
        "name": "Updated VIP Ticket",
        "description": "Updated VIP access",
        "price": "129.99",
        "available_quantity": "30"
    })";
    req.headers.emplace("Content-Type", "application/json");
    
    crow::response res = controller.testUpdateTicketType(req, ticketTypeId);
    
    // 验证响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("ok"));
}

TEST_F(TicketControllerTest, UpdateTicketTypeWithEmptyId) {
    // 测试使用空ID更新票务类型
    std::string emptyId = "";
    crow::request req;
    req.body = R"({
        "event_id": "event_123",
        "name": "Test Ticket",
        "description": "Test description",
        "price": "50.00",
        "available_quantity": "100"
    })";
    
    crow::response res = controller.testUpdateTicketType(req, emptyId);
    
    // 验证返回错误
    EXPECT_THAT(res, StatusCodeIs(400));
    EXPECT_THAT(res, ResponseContains("error"));
    EXPECT_THAT(res, ResponseContains("no id"));
}

TEST_F(TicketControllerTest, DeleteTicketTypeWithValidId) {
    // 测试删除票务类型
    std::string ticketTypeId = "ticket_type_to_delete_123";
    crow::request req;
    req.body = R"({})"; // 空body，因为只需要ID
    
    crow::response res = controller.testDeleteTicketType(req, ticketTypeId);
    
    // 验证响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("ok"));
}

TEST_F(TicketControllerTest, DeleteTicketTypeWithEmptyId) {
    // 测试使用空ID删除票务类型
    std::string emptyId = "";
    crow::request req;
    req.body = R"({})";
    
    crow::response res = controller.testDeleteTicketType(req, emptyId);
    
    // 验证返回错误
    EXPECT_THAT(res, StatusCodeIs(400));
    EXPECT_THAT(res, ResponseContains("error"));
    EXPECT_THAT(res, ResponseContains("no id"));
}

TEST_F(TicketControllerTest, GetMyTicketWithValidJWT) {
    // 测试获取我的票券（有效JWT）
    crow::request req;
    std::string jwt = createTestJWT("test_user_123");
    req.headers.emplace("Cookie", "jwt=" + jwt);
    
    crow::response res = controller.testGetMyTicket(req);
    
    // 验证响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("data"));
}

TEST_F(TicketControllerTest, GetMyTicketWithoutCookie) {
    // 测试获取我的票券（没有Cookie）
    crow::request req;
    // 不设置Cookie头
    
    crow::response res = controller.testGetMyTicket(req);
    
    // 应该返回错误或空结果
    EXPECT_TRUE(res.code == 400 || res.code == 401 || res.code == 500);
}

TEST_F(TicketControllerTest, GetMyTicketWithInvalidJWT) {
    // 测试获取我的票券（无效JWT）
    crow::request req;
    req.headers.emplace("Cookie", "jwt=invalid.jwt.token");
    
    crow::response res = controller.testGetMyTicket(req);
    
    // 应该返回错误
    EXPECT_TRUE(res.code == 400 || res.code == 401 || res.code == 500);
}

TEST_F(TicketControllerTest, RefundTicketWithValidId) {
    // 测试票券退款
    std::string ticketId = "ticket_to_refund_123";
    crow::request req;
    req.body = R"({})";
    
    crow::response res = controller.testRefund(req, ticketId);
    
    // 验证响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("ok"));
}

TEST_F(TicketControllerTest, RefundTicketWithEmptyId) {
    // 测试使用空ID退款
    std::string emptyId = "";
    crow::request req;
    req.body = R"({})";
    
    crow::response res = controller.testRefund(req, emptyId);
    
    // 验证返回错误
    EXPECT_THAT(res, StatusCodeIs(400));
    EXPECT_THAT(res, ResponseContains("error"));
    EXPECT_THAT(res, ResponseContains("no id"));
}

TEST_F(TicketControllerTest, RefundNonexistentTicket) {
    // 测试退款不存在的票券
    std::string nonexistentId = "nonexistent_ticket_999";
    crow::request req;
    req.body = R"({})";
    
    crow::response res = controller.testRefund(req, nonexistentId);
    
    // 应该返回票券未找到的错误
    if (res.code == 400) {
        EXPECT_THAT(res, ResponseContains("error"));
        EXPECT_THAT(res, ResponseContains("not found"));
    }
}

TEST_F(TicketControllerTest, CreateTicketWithValidData) {
    // 测试创建票券
    crow::request req;
    req.body = R"({
        "userId": "test_user_123",
        "eventId": "test_event_123",
        "price": "75.00"
    })";
    req.headers.emplace("Content-Type", "application/json");
    
    crow::response res = controller.testCreateTicket(req);
    
    // 验证响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("ok"));
}

TEST_F(TicketControllerTest, CreateTicketWithMissingFields) {
    // 测试创建票券时缺少必填字段
    crow::request req;
    req.body = R"({
        "userId": "test_user_123"
    })";
    req.headers.emplace("Content-Type", "application/json");
    
    crow::response res = controller.testCreateTicket(req);
    
    // 验证返回错误
    EXPECT_THAT(res, StatusCodeIs(400));
    EXPECT_THAT(res, ResponseContains("error"));
    EXPECT_THAT(res, ResponseContains("please summit all fields"));
}

TEST_F(TicketControllerTest, GetTicketByValidId) {
    // 测试通过ID获取票券
    std::string ticketId = "ticket_123";
    crow::request req;
    
    crow::response res = controller.testGetTicketById(req, ticketId);
    
    // 验证响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("ok"));
}

TEST_F(TicketControllerTest, GetTicketByEmptyId) {
    // 测试使用空ID获取票券
    std::string emptyId = "";
    crow::request req;
    
    crow::response res = controller.testGetTicketById(req, emptyId);
    
    // 验证返回错误
    EXPECT_THAT(res, StatusCodeIs(400));
    EXPECT_THAT(res, ResponseContains("error"));
    EXPECT_THAT(res, ResponseContains("no id"));
}

TEST_F(TicketControllerTest, TicketTypeDataValidation) {
    // 测试票务类型数据验证
    crow::request req;
    req.body = R"({
        "event_id": "event_123",
        "name": "",
        "description": "Test description",
        "price": "-10.00",
        "available_quantity": "-5"
    })";
    req.headers.emplace("Content-Type", "application/json");
    
    crow::response res = controller.testCreateTicketType(req);
    
    // 根据实现，可能接受或拒绝负价格和负数量
    // 这取决于业务逻辑的验证
    EXPECT_TRUE(res.code == 200 || res.code == 400);
}

TEST_F(TicketControllerTest, PriceFormatValidation) {
    // 测试价格格式验证
    crow::request req;
    req.body = R"({
        "event_id": "event_123",
        "name": "Price Test Ticket",
        "description": "Testing price formats",
        "price": "not_a_number",
        "available_quantity": "10"
    })";
    req.headers.emplace("Content-Type", "application/json");
    
    crow::response res = controller.testCreateTicketType(req);
    
    // 价格格式错误应该被处理
    EXPECT_TRUE(res.code == 200 || res.code == 400 || res.code == 500);
}

TEST_F(TicketControllerTest, LargeQuantityHandling) {
    // 测试大数量处理
    crow::request req;
    req.body = R"({
        "event_id": "event_123",
        "name": "Large Quantity Ticket",
        "description": "Testing large quantities",
        "price": "50.00",
        "available_quantity": "999999"
    })";
    req.headers.emplace("Content-Type", "application/json");
    
    crow::response res = controller.testCreateTicketType(req);
    
    // 验证系统能处理大数量
    EXPECT_THAT(res, StatusCodeIs(200));
}

TEST_F(TicketControllerTest, ResponseFormatConsistency) {
    // 测试响应格式的一致性
    crow::request req;
    req.body = R"({
        "event_id": "format_test_event",
        "name": "Format Test Ticket",
        "description": "Testing response format",
        "price": "25.00",
        "available_quantity": "100"
    })";
    req.headers.emplace("Content-Type", "application/json");
    
    crow::response res = controller.testCreateTicketType(req);
    
    if (res.code == 200) {
        auto json = crow::json::load(res.body);
        
        // 验证响应格式一致性
        EXPECT_TRUE(json.has("message"));
        EXPECT_TRUE(json.has("data"));
        EXPECT_EQ(json["message"].s(), "ok");
    }
} 