#pragma once

#include <string>

enum class HTTP_METHOD {
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

enum class CHECK_STATE {
    REQUESTLINE = 0,
    HEADER,
    CONTENT
};

enum class HTTP_CODE {
    NO_REQUEST,
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURCE,
    FORBIDDEN_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};

enum LINE_STATUS {
    OK = 0,
    BAD,
    OPEN
};

struct HttpConfig {
    bool linger;
    std::string version;
    HTTP_CODE reqRes;
    std::string url;
};

constexpr int READ_BUFFER_SIZE = 2048;

class HttpReq {
public:
    void Reset();
    bool ReadOnce(int sockFd);
    HTTP_CODE DoReq();
    HTTP_CODE ProcessRead();

    std::string m_url;
    bool m_linger;
    std::string m_version;

private:
    char m_readBuf[READ_BUFFER_SIZE];
    int m_readIdx;
    HTTP_METHOD m_method;

    std::string m_host;
    int m_contentLen;
    std::string m_body;
};
