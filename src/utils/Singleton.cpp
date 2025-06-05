#include "utils/Singleton.h"
#include <vector>
#include <functional>
#include "crow/middlewares/cors.h"

crow::App<crow::CORSHandler, AuthMiddleware>& getApp()
{
  static crow::App<crow::CORSHandler, AuthMiddleware> app;
  return app;
}
crow::Blueprint& getBlueprint()
{
  static crow::Blueprint bp("api");
  return bp;
}
