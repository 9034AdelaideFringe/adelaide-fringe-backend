#pragma once

#include "crow.h"
#include <string>

class UploadService
{
public:
    static std::string uploadFile(const std::pair<const std::string_view, crow::multipart::part_view> &part);

    static std::string uploadFile(const crow::multipart::part_view& partView);
};