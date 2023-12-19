#pragma once

#include "http_req.h"
#include "http_rsp.h"

class HttpData {
public:
    void CloseConn();
    HTTP_CODE DoReq();

    HttpReq m_req;
    HttpRsp m_rsp;

private:
    std::string m_realFile;
};
