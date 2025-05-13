#pragma once
#include "crow.h"

using namespace std;
using namespace crow;
class ExceptionHandler
{
public:
    static void defaultExceptionHandler(response& res);
};

