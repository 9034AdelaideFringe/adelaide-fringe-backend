#pragma once

#include <crow.h>
#include <string>

using namespace std;

struct AdelaideCORS
{
  struct context
  {
  };

  void before_handle(crow::request &req, crow::response &res, context &ctx)
  {
  }

  void after_handle(crow::request &req, crow::response &res, context &ctx)
  {
    auto &origin = req.get_header_value("Origin");
    string originFill = origin.empty() ? "null" : origin;
    res.set_header("Access-Control-Allow-Origin", move(originFill));
    res.set_header("Access-Control-Allow-Methods", "*");
    res.set_header("Access-Control-Allow-Credentials", "true");

  }
};