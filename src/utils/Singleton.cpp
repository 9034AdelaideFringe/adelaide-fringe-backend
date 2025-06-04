#include "utils/Singleton.h"
#include <vector>
#include <functional>
#include "crow/middlewares/cors.h"
#include "middlewares/AdelaideCORS.h"

crow::App<AdelaideCORS, AuthMiddleware>& getApp()
{
  static crow::App<AdelaideCORS, AuthMiddleware> app;
  return app;
}

crow::Blueprint& getBlueprint()
{
  static crow::Blueprint bp("api");
  return bp;
}