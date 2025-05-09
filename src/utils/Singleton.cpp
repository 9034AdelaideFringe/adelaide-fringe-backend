#include "utils/Singleton.h"
#include <vector>
#include <functional>
#include "crow/middlewares/cors.h"

crow::App<crow::CORSHandler>& getApp()
{
  // static crow::SimpleApp app;
  static crow::App<crow::CORSHandler> app;
  return app;
}

crow::Blueprint& getBlueprint()
{
  static crow::Blueprint bp("api");
  return bp;
}