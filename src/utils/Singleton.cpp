#include "utils/Singleton.h"
#include <vector>
#include <functional>
#include "crow/middlewares/cors.h" // 包含 Crow 内置的 CORS 中间件头文件
#include "middlewares/AuthMiddleware.h" // 确保 AuthMiddleware 仍然包含

// 将 App 的模板参数设置为 crow::CORSHandler 和 AuthMiddleware
crow::App<crow::CORSHandler, AuthMiddleware>& getApp()
{
  static crow::App<crow::CORSHandler, AuthMiddleware> app;

  // --- 配置 Crow 内置的 CORS 中间件 ---
  // 获取 CORS 中间件的全局规则对象
  auto& cors_rules = app.get_middleware<crow::CORSHandler>().global();

  // 直接设置 CORSRules 对象的成员来配置规则
  cors_rules.allowed_origins = {
      "https://frontend-psi-blond.vercel.app",
      "http://localhost:5173"
      // 如果有其他允许的来源，可以在这里添加
  };

  cors_rules.allowed_methods = "POST, GET, PUT, DELETE, OPTIONS"; // 允许常用的方法
  cors_rules.allowed_headers = "Content-Type, Authorization, Accept, Cookie, X-Requested-With"; // 允许常用的头部
  cors_rules.allow_credentials = true; // 允许凭证 (如 cookies)
  cors_rules.max_age = 86400; // 预检请求的缓存时间 (可选，单位秒)
  // --- 配置结束 ---


  return app;
}

crow::Blueprint& getBlueprint()
{
  static crow::Blueprint bp("api");
  return bp;
}
