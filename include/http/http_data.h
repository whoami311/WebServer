#ifndef HTTP_DATA_H
#define HTTP_DATA_H

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "http_req.h"
#include "http_rsp.h"

using namespace std;

class HttpData {
public:
    const int FILENAME_LEN = 200;

    void init();
    void CloseConn();
    E_HTTP_CODE DoReq();

    HttpReq m_req;
    HttpRsp m_rsp;
private:
    string m_realFile;
};

#endif
