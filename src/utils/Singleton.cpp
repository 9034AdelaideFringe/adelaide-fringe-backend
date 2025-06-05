#include "utils/Singleton.h"
#include <vector>
#include <functional>
#include "crow/middlewares/cors.h" // 包含 Crow 内置的 CORS 中间件头文件
#include "middlewares/AuthMiddleware.h" // 确保 AuthMiddleware 仍然包含

// 将 App 的模板参数改回 crow::CORSHandler
crow::App<crow::CORSHandler, AuthMiddleware>& getApp()
{
  static crow::App<crow::CORSHandler, AuthMiddleware> app;

  // --- 配置 Crow 内置的 CORS 中间件 ---
  auto& cors = app.get_middleware<crow::CORSHandler>();

  // 允许特定来源
  cors
    .allow_origin("https://frontend-psi-blond.vercel.app")
    .allow_origin("http://localhost:5173");
    // 如果有其他允许的来源，可以在这里继续调用 .allow_origin(...)

  // 允许的方法
  cors.allow_methods("POST, GET, PUT, DELETE, OPTIONS");
  // 或者如果需要允许所有方法，可以使用 cors.allow_methods("*");

  // 允许的头部
  cors.allow_headers("Content-Type, Authorization, Accept, Cookie, X-Requested-With");
  // 或者如果需要允许所有头部，可以使用 cors.allow_headers("*");

  // 允许凭证 (如 cookies)
  cors.allow_credentials(true);

  // 设置预检请求的缓存时间 (可选，单位秒)
  cors.max_age(86400); // 24小时
  // --- 配置结束 ---


  return app;
}

crow::Blueprint& getBlueprint()
{
  static crow::Blueprint bp("api");
  return bp;
}
