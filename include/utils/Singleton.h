#pragma once

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "middlewares/AuthMiddleware.h"

crow::App<crow::CORSHandler, AuthMiddleware>& getApp();

crow::Blueprint& getBlueprint();