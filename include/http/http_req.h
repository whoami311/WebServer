#ifndef HTTP_REQ_H
#define HTTP_REQ_H

#include <string.h>
#include <sys/socket.h>
#include "string"
#include "log.h"
#include "utils.h"
#include "http_parse.h"

using namespace std;

enum E_HTTP_METHOD {
    GET = 0,
    POST,
    HEAD,
    PUT,
    DELETE,
    TRACE,
    OPTIONS,
    CONNECT,
    PATH
};

enum E_CHECK_STATE {
    CHECK_STATE_REQUESTLINE = 0,
    CHECK_STATE_HEADER,
    CHECK_STATE_CONTENT
};

enum E_HTTP_CODE {
    NO_REQUEST,
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURCE,
    FORBIDDEN_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};

enum E_LINE_STATUS {
    LINE_OK = 0,
    LINE_BAD,
    LINE_OPEN
};

struct HttpConfig {
    bool linger;
    string version;
    E_HTTP_CODE reqRes;
    string url;
};

class HttpReq {
public:
    static const int READ_BUFFER_SIZE = 2048;

    void Reset();
    bool ReadOnce(int sockFd);
    E_HTTP_CODE DoReq();
    E_HTTP_CODE ProcessRead();

    string m_url;
    bool m_linger;
    string m_version;
private:
    char m_readBuf[READ_BUFFER_SIZE];
    int m_readIdx;
    E_HTTP_METHOD m_method;
    
    string m_host;
    int m_contentLen;
    string m_body;
};

#endif
