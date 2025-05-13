#include "exception/ExceptionHandler.h"
#include <typeinfo>

void ExceptionHandler::defaultExceptionHandler(response &res)
{
    try
    {
        throw;
    }
    catch(const exception& e)
    {
        CROW_LOG_ERROR << "Caught exception, " << typeid(e).name() << ": " << e.what();
        res = json::wvalue{{"error", e.what()}};
    }
    catch(...)
    {
        CROW_LOG_ERROR << "Unknown error happened";
        res = json::wvalue{{"error", "unknown server error"}};
    }
}