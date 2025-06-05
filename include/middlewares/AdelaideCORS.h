#pragma once

#include <crow.h>
#include <string>
#include <vector>
#include <algorithm> // 用于 std::find

using namespace std;

struct AdelaideCORS
{
  struct context
  {
  };

  // 定义允许的前端来源列表
  const vector<string> allowed_origins = {
      "https://frontend-psi-blond.vercel.app",
      "http://localhost:5173" // 您的本地开发地址
      // 如果有其他允许的来源，可以在这里添加
  };

  void before_handle(crow::request &req, crow::response &res, context &ctx)
  {
      string origin = req.get_header_value("Origin");
      // CROW_LOG_INFO << "CORS (before): Received Origin header: '" << origin << "'"; // 可选日志

      // 检查请求的 Origin 是否在允许列表中
      bool is_allowed_origin = (!origin.empty() &&
                                std::find(allowed_origins.begin(), allowed_origins.end(), origin) != allowed_origins.end());

      // CROW_LOG_INFO << "CORS (before): Origin '" << origin << "' is allowed: " << (is_allowed_origin ? "true" : "false"); // 可选日志


      // 处理 CORS 预检请求 (OPTIONS 方法)
      if (req.method == crow::HTTPMethod::OPTIONS) {
          // CROW_LOG_INFO << "CORS (before): Handling OPTIONS request."; // 可选日志

          if (is_allowed_origin) {
              // CROW_LOG_INFO << "CORS (before): Origin is allowed. Setting CORS headers for OPTIONS."; // 可选日志
              // 设置 Access-Control-Allow-Origin 为请求的实际 Origin
              res.set_header("Access-Control-Allow-Origin", origin);

              // 必须设置 Access-Control-Allow-Methods，列出所有允许的方法 (修正语法)
              res.set_header("Access-Control-Allow-Methods", "POST, GET, PUT, DELETE, OPTIONS");

              // 必须设置 Access-Control-Allow-Headers，列出前端可能发送的所有头部 (修正语法)
              res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept, Cookie, X-Requested-With");

              // 如果允许携带凭证 (如 cookies)，必须设置为 true
              res.set_header("Access-Control-Allow-Credentials", "true");

              // 设置预检请求的缓存时间 (可选，单位秒)
              res.set_header("Access-Control-Max-Age", "86400"); // 24小时

              // 预检请求处理完毕，直接返回 204 No Content
              res.code = 204;
              res.end(); // 结束请求处理流程
          } else {
              // 如果是 OPTIONS 请求但来源不被允许，返回 403 Forbidden
              CROW_LOG_WARNING << "CORS (before): Forbidden OPTIONS request from origin: " << origin;
              res.code = 403;
              res.end();
          }
      }
      // 如果不是 OPTIONS 请求，继续到 after_handle
  }

  void after_handle(crow::request &req, crow::response &res, context &ctx)
  {
      // 对于非预检请求 (实际请求)，也需要设置 CORS 头部
      // 注意：OPTIONS 请求已经在 before_handle 中处理并结束，不会到达这里
      string origin = req.get_header_value("Origin");
      // CROW_LOG_INFO << "CORS (after): Received Origin header: '" << origin << "'"; // 可选日志

      // 检查请求的 Origin 是否在允许列表中
      bool is_allowed_origin = (!origin.empty() &&
                                std::find(allowed_origins.begin(), allowed_origins.end(), origin) != allowed_origins.end());

      // CROW_LOG_INFO << "CORS (after): Origin '" << origin << "' is allowed: " << (is_allowed_origin ? "true" : "false"); // 可选日志

      // 如果来源被允许，设置 Access-Control-Allow-Origin
      if (is_allowed_origin) {
          // CROW_LOG_INFO << "CORS (after): Origin is allowed. Setting CORS headers for actual request."; // 可选日志
          res.set_header("Access-Control-Allow-Origin", origin);
          // 设置其他必要的 CORS 头部 (修正语法)
          res.set_header("Access-Control-Allow-Methods", "POST, GET, PUT, DELETE, OPTIONS"); // <-- 修正这里
          res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept, Cookie, X-Requested-With"); // <-- 修正这里
          res.set_header("Access-Control-Allow-Credentials", "true");
      } else if (!origin.empty()) {
          // 如果来源不被允许且 Origin 头部存在，记录警告
          CROW_LOG_WARNING << "CORS (after): Actual request from disallowed origin: " << origin;
          // 对于实际请求，如果来源不被允许，浏览器通常会阻止，
          // 但后端也可以选择明确返回 403 Forbidden
          // res.code = 403; res.end(); return;
      } else {
           // Origin 头部为空的情况
           CROW_LOG_WARNING << "CORS (after): Actual request with empty Origin header from " << req.remote_ip_address;
      }
  }
};
