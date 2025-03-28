#include "utils/Singleton.h"
#include <vector>
#include <functional>

crow::SimpleApp& getApp()
{
  static crow::SimpleApp app;
  return app;
}