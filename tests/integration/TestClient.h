#pragma once

#include <string>
#include <map>
#include "crow.h"

// 模拟HTTP客户端，用于集成测试
class TestClient {
public:
    // 构造一个GET请求
    static crow::request makeGetRequest(const std::string& url, 
                                       const std::map<std::string, std::string>& headers = {}) {
        crow::request req;
        req.url = url;
        req.method = crow::HTTPMethod::Get;
        
        for (const auto& [key, value] : headers) {
            req.headers.emplace(key, value);
        }
        
        return req;
    }
    
    // 构造一个POST请求
    static crow::request makePostRequest(const std::string& url, 
                                        const std::string& body,
                                        const std::map<std::string, std::string>& headers = {}) {
        crow::request req;
        req.url = url;
        req.method = crow::HTTPMethod::Post;
        req.body = body;
        
        for (const auto& [key, value] : headers) {
            req.headers.emplace(key, value);
        }
        
        // 默认设置Content-Type为application/json
        if (req.headers.find("Content-Type") == req.headers.end()) {
            req.headers.emplace("Content-Type", "application/json");
        }
        
        return req;
    }
    
    // 构造一个PUT请求
    static crow::request makePutRequest(const std::string& url, 
                                       const std::string& body,
                                       const std::map<std::string, std::string>& headers = {}) {
        crow::request req;
        req.url = url;
        req.method = crow::HTTPMethod::Put;
        req.body = body;
        
        for (const auto& [key, value] : headers) {
            req.headers.emplace(key, value);
        }
        
        // 默认设置Content-Type为application/json
        if (req.headers.find("Content-Type") == req.headers.end()) {
            req.headers.emplace("Content-Type", "application/json");
        }
        
        return req;
    }
    
    // 构造一个DELETE请求
    static crow::request makeDeleteRequest(const std::string& url, 
                                          const std::map<std::string, std::string>& headers = {}) {
        crow::request req;
        req.url = url;
        req.method = crow::HTTPMethod::Delete;
        
        for (const auto& [key, value] : headers) {
            req.headers.emplace(key, value);
        }
        
        return req;
    }
    
    // 解析响应中的JSON
    static crow::json::rvalue parseResponseJson(const crow::response& res) {
        return crow::json::load(res.body);
    }
    
    // 从Cookie中提取JWT令牌
    static std::string extractJwtFromCookie(const crow::response& res) {
        auto it = res.headers.find("Set-Cookie");
        if (it == res.headers.end()) {
            return "";
        }
        
        std::string cookie = it->second;
        size_t jwtPos = cookie.find("jwt=");
        if (jwtPos == std::string::npos) {
            return "";
        }
        
        jwtPos += 4; // "jwt="的长度
        size_t endPos = cookie.find(";", jwtPos);
        
        if (endPos == std::string::npos) {
            return cookie.substr(jwtPos);
        } else {
            return cookie.substr(jwtPos, endPos - jwtPos);
        }
    }
}; 