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
      // 将字符串比较改为枚举比较
      if (req.method == crow::HTTPMethod::OPTIONS) { // <-- 问题在这里，需要修改
          string origin = req.get_header_value("Origin");
          string originFill = (origin.empty() ? "null" : origin);
          
          res.set_header("Access-Control-Allow-Origin", originFill);
          res.set_header("Access-Control-Allow-Methods", "POST, GET, PUT, DELETE, OPTIONS"); 
          res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept, Cookie, X-Requested-With"); 
          res.set_header("Access-Control-Allow-Credentials", "true");
          res.set_header("Access-Control-Max-Age", "86400"); 
          
          res.code = 204; 
          res.end(); 
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
