#pragma once

#include "crow.h"
#include <pqxx/pqxx>

crow::json::wvalue resultToJSON(const pqxx::row& row);
crow::json::wvalue resultsToJSON(const pqxx::result& r);