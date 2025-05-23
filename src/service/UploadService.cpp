#include "service/UploadService.h"
#include <filesystem>
#include <fstream>
#include "utils/uuid.h"
#include "exception/FileUploadException.h"

using namespace std;

std::string UploadService::uploadFile(const std::pair<const std::string_view, crow::multipart::part_view> &part)
{
    return uploadFile(part);

    
}

std::string UploadService::uploadFile(const crow::multipart::part_view& partView)
{
    auto headersIt = partView.headers.find("Content-Disposition");
    if(headersIt == partView.headers.end())
    {
        throw FileUploadException("missing Content-Disposition");
    }
    auto paramsIt = headersIt->second.params.find("filename");
    if(paramsIt == headersIt->second.params.end())
    {
        throw FileUploadException("missing filename");
    }
    
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


    CROW_LOG_INFO << "uploadFile: " << fileName;


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