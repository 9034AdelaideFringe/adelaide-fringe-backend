#pragma once

#include "crow.h"
#include "middlewares/AdelaideCORS.h"
#include "middlewares/AuthMiddleware.h"

crow::App<AdelaideCORS, AuthMiddleware>& getApp();

crow::Blueprint& getBlueprint();