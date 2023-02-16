#include "http/http_req.h"

using namespace std;

void HttpReq::Reset() {
    m_method = GET;
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

E_HTTP_CODE HttpReq::ProcessRead() {
    HttpParser parseTxt(m_readBuf);
    // parseTxt.show();
    m_url = parseTxt["Path"];
    if (parseTxt["Method"] == "GET") {
        m_method = GET;
    } else {
        m_method = POST;
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
    cout << "m_url == "<< m_url << ", m_method = "<< m_method << ", m_body = " << m_body << endl;
    
    if (m_url.empty() || m_version.empty())
        return BAD_REQUEST;
    if (m_method == GET || !m_body.empty())
        return GET_REQUEST;

    return NO_REQUEST;
}
