#pragma once

#include "crow.h"
#include "crow/middlewares/cors.h"

crow::App<crow::CORSHandler>& getApp();
