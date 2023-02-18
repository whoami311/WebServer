#ifndef HTTP_RSP_H
#define HTTP_RSP_H

#include <unistd.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "string"
#include "log.h"
#include "http_req.h"
#include "http/http_build.h"

using namespace std;

class HttpRsp {
public:
    HttpRsp();
    ~HttpRsp();
    
    static const int WRITE_BUFFER_SIZE = 1024;

    void init();
    bool ProcessWrite(HttpConfig config);
    void unmap();
    // bool Write();

    char *m_fileAddr;
    struct stat m_fileStat;
    string fileContent;
    HttpBuild m_builder;
private:
};

#endif