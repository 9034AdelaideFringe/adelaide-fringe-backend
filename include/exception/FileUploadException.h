#pragma once

#include <string>
#include <stdexcept>

using namespace std;

class FileUploadException : public runtime_error
{
public:
    FileUploadException(const string& msg) : runtime_error(msg){}
}; 