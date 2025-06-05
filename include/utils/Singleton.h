#pragma once

#include "crow.h"
#include "crow/middlewares/cors.h" // 包含 Crow 内置的 CORS 中间件头文件
#include "middlewares/AuthMiddleware.h"

// 将 App 的模板参数改回 crow::CORSHandler
crow::App<crow::CORSHandler, AuthMiddleware>& getApp();

crow::Blueprint& getBlueprint();
