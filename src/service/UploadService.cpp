#include "service/UploadService.h"
#include <filesystem>
#include <fstream>
#include "utils/uuid.h"

using namespace std;

std::string UploadService::uploadFile(const std::pair<const std::string_view, crow::multipart::part_view> &part)
{
    return uploadFile(part);

    // const auto& partName = part.first;
    // const auto& partValue = part.second;

    // CROW_LOG_INFO << "uploadFile: " << partName;

    // auto headersIt = partValue.headers.find("Content-Disposition");
    // auto paramsIt = headersIt->second.params.find("filename");
    // const std::string fileName(paramsIt->second);

    // const auto& uuid = generateUUID();


    

    // std::filesystem::path dir("./images");

    // if(!std::filesystem::exists(dir))
    // {
    //     CROW_LOG_INFO << "dir ./images doesn't exist, creating...";
    //     std::filesystem::create_directory(dir);
    // }

    // std::filesystem::path fullPath = dir / uuid;

    // std::ofstream outStream(fullPath);
    // outStream << partValue.body;

    // outStream.close();

    // CROW_LOG_INFO << "file: " << fileName << " has been written to " << fullPath.c_str();

    // std::string result = fullPath.string();
    // return result;
}

std::string UploadService::uploadFile(const crow::multipart::part_view& partView)
{
    auto headersIt = partView.headers.find("Content-Disposition");
    auto paramsIt = headersIt->second.params.find("filename");
    std::string fileName(paramsIt->second);
    string extName;

    auto pos = fileName.find_last_of('.');
    if(pos != string::npos)
    {
        extName = fileName.substr(pos);
    }
    else
    {
        extName = ".png";
    }

    // const auto& partName = part.first;
    // const auto& partValue = part.second;

    CROW_LOG_INFO << "uploadFile: " << fileName;

    // auto headersIt = partValue.headers.find("Content-Disposition");
    // auto paramsIt = headersIt->second.params.find("filename");
    // const std::string fileName(paramsIt->second);

    const auto& uuid = generateUUID();

    std::filesystem::path dir("./images");

    if(!std::filesystem::exists(dir))
    {
        CROW_LOG_INFO << "dir ./images doesn't exist, creating...";
        std::filesystem::create_directory(dir);
    }

    std::filesystem::path fullPath = dir / (uuid + extName);

    std::ofstream outStream(fullPath);
    outStream << partView.body;

    outStream.close();

    CROW_LOG_INFO << "file: " << fileName << " has been written to " << fullPath.c_str();

    std::string result = fullPath.string();
    return result;
}