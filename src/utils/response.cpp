#include "utils/response.h"

crow::json::wvalue resultToJSON(const pqxx::row& row)
{
    crow::json::wvalue object;
    for(const auto& field : row)
    {
        std::string name = field.name();
        std::string value = field.as<std::string>();
        object[name] = value;
    }
    return object;
}


crow::json::wvalue resultsToJSON(const pqxx::result &r)
{
    std::vector<crow::json::wvalue> objs;
    for (int i = 0; i < r.size(); i++)
    {
        pqxx::row row = r.at(i);
        auto obj = resultToJSON(row);
        objs.push_back(obj);
    }
    crow::json::wvalue ret = std::move(objs);
    return ret;
}