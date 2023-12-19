#pragma once

#include <sys/stat.h>

#include <string>

#include "http/http_build.h"
#include "http/http_req.h"

class HttpRsp {
public:
    HttpRsp();
    ~HttpRsp();

    // static const int WRITE_BUFFER_SIZE = 1024;

    bool ProcessWrite(const HttpConfig& config);
    void unmap();
    // bool Write();

    char* m_fileAddr;
    struct stat m_fileStat;
    std::string fileContent;
    HttpBuild m_builder;
};
