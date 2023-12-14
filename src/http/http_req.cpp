#include "http/http_req.h"

#include <string.h>
#include <sys/socket.h>

#include "http/http_parse.h"
#include "log.h"
#include "utils.h"

void HttpReq::Reset() {
    m_method = HTTP_METHOD::GET;
    m_url = "";
    m_version = "";
    m_contentLen = 0;
    m_host = "";
    m_readIdx = 0;

    bzero(m_readBuf, sizeof(m_readBuf));
}

bool HttpReq::ReadOnce(int sockFd) {
    // if (m_readIdx >= READ_BUFFER_SIZE)
    //     return false;
    m_readIdx = 0;
    int bytesRead = 0;

    while (true) {
        bytesRead = recv(sockFd, m_readBuf + m_readIdx, READ_BUFFER_SIZE - m_readIdx, 0);
        if (bytesRead == -1) {
            LOG_WARN("recv failed");
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            return false;
        } else if (bytesRead == 0)
            return false;
        m_readIdx += bytesRead;
    }

    LOG_INFO("m_readBuf == %s", m_readBuf);
    return true;
}

HTTP_CODE HttpReq::ProcessRead() {
    HttpParser parseTxt(m_readBuf);
    // parseTxt.Show();
    m_url = parseTxt["Path"];
    if (parseTxt["Method"] == "GET") {
        m_method = HTTP_METHOD::GET;
    } else {
        m_method = HTTP_METHOD::POST;
    }
    m_version = parseTxt["Version"];
    if (!parseTxt["Content-length"].empty())
        m_contentLen = stoi(parseTxt["Content-length"]);
    m_host = parseTxt["Host"];
    if (parseTxt["Connection"] == "keep-alive\r") {
        m_linger = true;
    } else {
        m_linger = false;
    }
    m_body = parseTxt["Body"];
    std::cout << "m_url == " << m_url << ", m_method = " << static_cast<int>(m_method) << ", m_body = " << m_body
              << std::endl;

    if (m_url.empty() || m_version.empty())
        return HTTP_CODE::BAD_REQUEST;
    if (m_method == HTTP_METHOD::GET || !m_body.empty())
        return HTTP_CODE::GET_REQUEST;

    return HTTP_CODE::NO_REQUEST;
}
