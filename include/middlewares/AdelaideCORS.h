#pragma once

#include <crow.h>
#include <string>

using namespace std;

struct AdelaideCORS
{
  struct context
  {
  };

  void before_handle(crow::request &req, crow::response &res, context &ctx)
  {
      // 处理 CORS 预检请求 (OPTIONS 方法)
      if (req.method == "OPTIONS") {
          string origin = req.get_header_value("Origin");
          string originFill = (origin.empty() ? "null" : origin);
          
          // 必须设置 Access-Control-Allow-Origin
          res.set_header("Access-Control-Allow-Origin", originFill);
          
          // 必须设置 Access-Control-Allow-Methods，列出所有允许的方法
          res.set_header("Access-Control-Allow-Methods", "POST, GET, PUT, DELETE, OPTIONS"); 
          
          // 必须设置 Access-Control-Allow-Headers，列出前端可能发送的所有头部
          // 根据您的前端需求添加或移除头部
          res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept, Cookie, X-Requested-With"); 
          
          // 如果允许携带凭证 (如 cookies)，必须设置为 true
          res.set_header("Access-Control-Allow-Credentials", "true");
          
          // 设置预检请求的缓存时间 (可选，单位秒)
          res.set_header("Access-Control-Max-Age", "86400"); // 24小时
          
          // 预检请求处理完毕，直接返回 204 No Content
          res.code = 204; 
          res.end(); // 结束请求处理流程
      }
  }

  void after_handle(crow::request &req, crow::response &res, context &ctx)
  {
      // 对于非预检请求 (实际请求)，也需要设置 CORS 头部
      // 注意：OPTIONS 请求已经在 before_handle 中处理并结束，不会到达这里
      string origin = req.get_header_value("Origin");
      string originFill = (origin.empty() ? "null" : origin);
      
      res.set_header("Access-Control-Allow-Origin", originFill);
      res.set_header("Access-Control-Allow-Methods", "POST, GET, PUT, DELETE, OPTIONS");
      res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept, Cookie, X-Requested-With");
      res.set_header("Access-Control-Allow-Credentials", "true");
  }
};
