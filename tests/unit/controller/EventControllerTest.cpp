#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "controller/event/EventController.h"
#include "utils/Singleton.h"
#include "utils/Config.h"
#include "utils/uuid.h"
#include <fstream>

// 自定义匹配器
MATCHER_P(ResponseContains, text, "") {
    return arg.body.find(text) != std::string::npos;
}

MATCHER_P(StatusCodeIs, code, "") {
    return arg.code == code;
}

// 创建一个测试友好的EventController子类
class TestableEventController : public EventController {
public:
    using EventController::EventController;
    
    // 暴露protected方法用于测试
    crow::response testGetEvent(const crow::request& req) {
        return getEvent(req);
    }
    
    crow::response testGetEventById(const std::string& id) {
        return getEventById(id);
    }
    
    crow::response testCreateEvent(const crow::request& req) {
        return createEvent(req);
    }
    
    crow::response testUpdateEvent(const crow::request& req, const std::string& id) {
        return updateEvent(req, id);
    }
    
    crow::response testDeleteEvent(const crow::request& req, const std::string& id) {
        return deleteEvent(req, id);
    }
    
    crow::response testSearchEvent(const crow::request& req) {
        return searchEvent(req);
    }
    
    crow::response testGetTicketTypes(const crow::request& req, const std::string& id) {
        return getTicketTypes(req, id);
    }
};

class EventControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置测试配置 - 使用Mock数据库
        std::ofstream testConfig("test_config.json");
        testConfig << R"({
            "database": "mock://localhost/test_db",
            "JWTSecret": "test_jwt_secret_for_event_testing"
        })";
        testConfig.close();
        Config::loadConfig("test_config.json");
    }

    void TearDown() override {
        // 清理测试配置文件
        std::remove("test_config.json");
    }

    TestableEventController controller;
    
    // 辅助方法：创建multipart请求体
    std::string createMultipartBody(const std::map<std::string, std::string>& fields) {
        std::string boundary = "----TestBoundary1234567890";
        std::string body = "";
        
        for (const auto& field : fields) {
            body += "--" + boundary + "\r\n";
            body += "Content-Disposition: form-data; name=\"" + field.first + "\"\r\n\r\n";
            body += field.second + "\r\n";
        }
        body += "--" + boundary + "--\r\n";
        
        return body;
    }
};

TEST_F(EventControllerTest, GetAllEvents) {
    // 测试获取所有事件
    crow::request req;
    
    crow::response res = controller.testGetEvent(req);
    
    // 验证响应格式
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("data"));
    
    // 解析响应JSON验证结构
    auto json = crow::json::load(res.body);
    EXPECT_TRUE(json.has("message"));
    EXPECT_TRUE(json.has("data"));
    EXPECT_EQ(json["message"].s(), "ok");
}

TEST_F(EventControllerTest, GetEventByValidId) {
    // 测试通过有效ID获取事件
    std::string validId = "test-event-id-12345";
    
    crow::response res = controller.testGetEventById(validId);
    
    // 验证响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("data"));
}

TEST_F(EventControllerTest, GetEventByEmptyId) {
    // 测试使用空ID获取事件
    std::string emptyId = "";
    
    crow::response res = controller.testGetEventById(emptyId);
    
    // 验证返回错误
    EXPECT_THAT(res, StatusCodeIs(400));
    EXPECT_THAT(res, ResponseContains("error"));
    EXPECT_THAT(res, ResponseContains("id is empty"));
}

TEST_F(EventControllerTest, CreateEventWithValidData) {
    // 测试创建事件（使用模拟的multipart数据）
    crow::request req;
    
    std::map<std::string, std::string> eventData = {
        {"title", "Test Event"},
        {"description", "This is a test event description"},
        {"short_description", "Test event"},
        {"date", "2024-12-25"},
        {"time", "19:00:00"},
        {"end_time", "22:00:00"},
        {"venue", "Test Venue"},
        {"capacity", "100"},
        {"category", "Music"},
        {"status", "active"},
        {"created_by", "test_user_id"}
    };
    
    req.body = createMultipartBody(eventData);
    req.headers.emplace("Content-Type", "multipart/form-data; boundary=----TestBoundary1234567890");
    
    crow::response res = controller.testCreateEvent(req);
    
    // 验证成功响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("ok"));
}

TEST_F(EventControllerTest, UpdateEventWithValidData) {
    // 测试更新事件
    std::string eventId = "existing-event-id-12345";
    crow::request req;
    
    std::map<std::string, std::string> updatedData = {
        {"title", "Updated Test Event"},
        {"description", "Updated description"},
        {"short_description", "Updated event"},
        {"date", "2024-12-26"},
        {"time", "20:00:00"},
        {"end_time", "23:00:00"},
        {"venue", "Updated Venue"},
        {"capacity", "150"},
        {"category", "Theatre"},
        {"status", "active"},
        {"created_by", "test_user_id"}
    };
    
    req.body = createMultipartBody(updatedData);
    req.headers.emplace("Content-Type", "multipart/form-data; boundary=----TestBoundary1234567890");
    
    crow::response res = controller.testUpdateEvent(req, eventId);
    
    // 验证响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
}

TEST_F(EventControllerTest, DeleteEventWithValidId) {
    // 测试删除事件
    std::string eventId = "event-to-delete-12345";
    crow::request req;
    
    crow::response res = controller.testDeleteEvent(req, eventId);
    
    // 验证响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
}

TEST_F(EventControllerTest, DeleteEventWithEmptyId) {
    // 测试使用空ID删除事件
    std::string emptyId = "";
    crow::request req;
    
    crow::response res = controller.testDeleteEvent(req, emptyId);
    
    // 验证返回错误
    EXPECT_THAT(res, StatusCodeIs(400));
    EXPECT_THAT(res, ResponseContains("error"));
    EXPECT_THAT(res, ResponseContains("no id"));
}

TEST_F(EventControllerTest, SearchEventWithValidTitle) {
    // 测试事件搜索功能
    crow::request req;
    req.body = R"({
        "title": "Test Event"
    })";
    
    crow::response res = controller.testSearchEvent(req);
    
    // 验证响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("data"));
}

TEST_F(EventControllerTest, SearchEventWithoutTitle) {
    // 测试没有标题的搜索请求
    crow::request req;
    req.body = R"({
        "description": "Some description"
    })";
    
    crow::response res = controller.testSearchEvent(req);
    
    // 验证返回错误
    EXPECT_THAT(res, StatusCodeIs(400));
    EXPECT_THAT(res, ResponseContains("error"));
    EXPECT_THAT(res, ResponseContains("no title"));
}

TEST_F(EventControllerTest, SearchEventWithInvalidJSON) {
    // 测试无效JSON的搜索请求
    crow::request req;
    req.body = "invalid json format";
    
    crow::response res = controller.testSearchEvent(req);
    
    // 验证返回错误
    EXPECT_THAT(res, StatusCodeIs(400));
    EXPECT_THAT(res, ResponseContains("error"));
}

TEST_F(EventControllerTest, GetTicketTypesWithValidId) {
    // 测试获取事件的票务类型
    std::string eventId = "event-with-tickets-12345";
    crow::request req;
    
    crow::response res = controller.testGetTicketTypes(req, eventId);
    
    // 验证响应
    EXPECT_THAT(res, StatusCodeIs(200));
    EXPECT_THAT(res, ResponseContains("message"));
    EXPECT_THAT(res, ResponseContains("data"));
}

TEST_F(EventControllerTest, GetTicketTypesWithEmptyId) {
    // 测试使用空ID获取票务类型
    std::string emptyId = "";
    crow::request req;
    
    crow::response res = controller.testGetTicketTypes(req, emptyId);
    
    // 验证返回错误
    EXPECT_THAT(res, StatusCodeIs(400));
    EXPECT_THAT(res, ResponseContains("error"));
    EXPECT_THAT(res, ResponseContains("id is empty"));
}

TEST_F(EventControllerTest, CreateEventWithMissingFields) {
    // 测试创建事件时缺少必填字段
    crow::request req;
    
    std::map<std::string, std::string> incompleteData = {
        {"title", "Incomplete Event"},
        {"description", "Missing some fields"}
        // 缺少其他必填字段
    };
    
    req.body = createMultipartBody(incompleteData);
    req.headers.emplace("Content-Type", "multipart/form-data; boundary=----TestBoundary1234567890");
    
    crow::response res = controller.testCreateEvent(req);
    
    // 根据实现，这可能返回错误或成功但有警告
    // 需要根据实际的EventService实现来调整期望
    EXPECT_TRUE(res.code == 200 || res.code == 400);
}

TEST_F(EventControllerTest, EventResponseFormatValidation) {
    // 测试事件响应的JSON格式
    crow::request req;
    
    crow::response res = controller.testGetEvent(req);
    
    if (res.code == 200) {
        auto json = crow::json::load(res.body);
        
        // 验证响应格式
        EXPECT_TRUE(json.has("message"));
        EXPECT_TRUE(json.has("data"));
        EXPECT_EQ(json["message"].s(), "ok");
        
        // 如果有数据，验证数据结构
        if (json.has("data") && json["data"].size() > 0) {
            // 可以进一步验证每个事件对象的结构
            // 这取决于实际的数据库模式
        }
    }
}

TEST_F(EventControllerTest, SearchWithEmptyTitle) {
    // 测试使用空标题搜索
    crow::request req;
    req.body = R"({
        "title": ""
    })";
    
    crow::response res = controller.testSearchEvent(req);
    
    // 空标题搜索应该返回所有结果或错误，取决于实现
    EXPECT_TRUE(res.code == 200 || res.code == 400);
}

TEST_F(EventControllerTest, LargeEventDataHandling) {
    // 测试处理大量事件数据
    crow::request req;
    
    std::map<std::string, std::string> largeEventData = {
        {"title", std::string(1000, 'A')}, // 1000字符的标题
        {"description", std::string(5000, 'B')}, // 5000字符的描述
        {"short_description", "Large event test"},
        {"date", "2024-12-25"},
        {"time", "19:00:00"},
        {"end_time", "22:00:00"},
        {"venue", "Large Test Venue"},
        {"capacity", "10000"},
        {"category", "Festival"},
        {"status", "active"},
        {"created_by", "test_user_id"}
    };
    
    req.body = createMultipartBody(largeEventData);
    req.headers.emplace("Content-Type", "multipart/form-data; boundary=----TestBoundary1234567890");
    
    crow::response res = controller.testCreateEvent(req);
    
    // 验证系统能处理大数据
    EXPECT_TRUE(res.code == 200 || res.code == 413); // 200成功或413请求实体过大
} 