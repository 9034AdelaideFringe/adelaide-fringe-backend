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
  // 获取 CORS 中间件实例并配置它
  app.get_middleware<crow::CORSHandler>()
      .global() // <-- 调用 .global() 获取配置器
      .allow_origin("https://frontend-psi-blond.vercel.app") // 允许第一个来源
      .allow_origin("http://localhost:5173") // 允许第二个来源
      // 如果有其他允许的来源，可以在这里继续调用 .allow_origin(...)

      .allow_methods("POST, GET, PUT, DELETE, OPTIONS") // 允许常用的方法
      // 如果需要允许所有方法，可以使用 .allow_methods("*")

      .allow_headers("Content-Type, Authorization, Accept, Cookie, X-Requested-With") // 允许常用的头部
      // 如果需要允许所有头部，可以使用 .allow_headers("*")

      .allow_credentials(true) // 允许凭证 (如 cookies)

      .max_age(86400); // 预检请求的缓存时间 (可选，单位秒)
  // --- 配置结束 ---


  return app;
}

crow::Blueprint& getBlueprint()
{
  static crow::Blueprint bp("api");
  return bp;
}
