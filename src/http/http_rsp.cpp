#include "http/http_rsp.h"

using namespace std;

HttpRsp::HttpRsp() {}

HttpRsp::~HttpRsp() {
    m_fileAddr = nullptr;
}

void HttpRsp::init() {
}

bool HttpRsp::ProcessWrite(HttpConfig config)
{
    E_HTTP_CODE ret = config.reqRes;
    m_builder.set("Version", config.version);
    m_builder.set("Server", "Felix Server");
    if (config.linger) {
        m_builder.set("Connection", "keep-alive");
        m_builder.set("Keep-Alive", "timeout=20");
    } else
        m_builder.set("Connection", "close");
    
    switch (ret) {
        case INTERNAL_ERROR: {
            m_builder.set("State", "500");
            return false;
            break;
        }
        case BAD_REQUEST: {
            m_builder.set("State", "404");
            return false;
            break;
        }
        case FORBIDDEN_REQUEST: {
            m_builder.set("State", "403");
            return false;
            break;
        }
        case FILE_REQUEST: {
            m_builder.set("State", "200");
            if (GetFileSuffix(config.url) == "ico")
                m_builder.set("Content-Type", "image/ico");
            else
                m_builder.set("Content-Type", "text/html");
            if (m_fileStat.st_size != 0) {
                m_builder.set("Content-Length", to_string(m_fileStat.st_size));
                return true;
            } else {
                const string okString = "<html><body></body></html>";
                m_builder.set("Body", okString);
                m_builder.set("Content-Length", to_string(okString.size()));
            }
        }
        default:
            return false;
    }

    return true;
}

void HttpRsp::unmap()
{
    if (m_fileAddr) {
        munmap(m_fileAddr, m_fileStat.st_size);
        m_fileAddr = nullptr;
    }
}

/* todo
bool HttpRsp::Write()
{
    int temp = 0;

    if (m_bytesToSend == 0)
    {
        EpollModFd(m_epollFd, m_sockFd, EPOLLIN);
        init();
        return true;
    }

    while (true)
    {
        temp = writev(m_sockFd, m_iv, m_ivCnt);

        if (temp < 0)
        {
            if (errno == EAGAIN) {
                EpollModFd(m_epollFd, m_sockFd, EPOLLOUT);
                return true;
            }
            unmap();
            return false;
        }

        m_bytesHaveSend += temp;
        m_bytesToSend -= temp;
        if (m_bytesHaveSend >= m_iv[0].iov_len)
        {
            m_iv[0].iov_len = 0;
            m_iv[1].iov_base = m_fileAddr + (m_bytesHaveSend - m_writeIdx);
            m_iv[1].iov_len = m_bytesToSend;
        }
        else
        {
            m_iv[0].iov_base = m_writeBuf + m_bytesHaveSend;
            m_iv[0].iov_len = m_iv[0].iov_len - m_bytesHaveSend;
        }

        if (m_bytesToSend <= 0)
        {
            unmap();
            EpollModFd(m_epollFd, m_sockFd, EPOLLIN);

            if (m_linger)
            {
                init();
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
*/

