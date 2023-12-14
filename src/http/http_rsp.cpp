#include "http/http_rsp.h"

#include <sys/mman.h>
#include <sys/uio.h>
#include <unistd.h>

#include "log.h"
#include "utils.h"

HttpRsp::HttpRsp() {}

HttpRsp::~HttpRsp() {
    m_fileAddr = nullptr;
}

void HttpRsp::Init() {}

bool HttpRsp::ProcessWrite(const HttpConfig &config) {
    HTTP_CODE ret = config.reqRes;
    m_builder.Set("Version", config.version);
    m_builder.Set("Server", "Felix Server");
    if (config.linger) {
        m_builder.Set("Connection", "keep-alive");
        m_builder.Set("Keep-Alive", "timeout=20");
    } else
        m_builder.Set("Connection", "close");

    switch (ret) {
        case HTTP_CODE::INTERNAL_ERROR: {
            m_builder.Set("State", "500");
            return false;
            break;
        }
        case HTTP_CODE::BAD_REQUEST: {
            m_builder.Set("State", "404");
            return false;
            break;
        }
        case HTTP_CODE::FORBIDDEN_REQUEST: {
            m_builder.Set("State", "403");
            return false;
            break;
        }
        case HTTP_CODE::FILE_REQUEST: {
            m_builder.Set("State", "200");
            if (utils::GetFileSuffix(config.url) == "ico")
                m_builder.Set("Content-Type", "image/ico");
            else
                m_builder.Set("Content-Type", "text/html");
            if (m_fileStat.st_size != 0) {
                m_builder.Set("Content-Length", std::to_string(m_fileStat.st_size));
                return true;
            } else {
                const std::string okString = "<html><body></body></html>";
                m_builder.Set("Body", okString);
                m_builder.Set("Content-Length", std::to_string(okString.size()));
            }
        }
        default:
            return false;
    }

    return true;
}

void HttpRsp::unmap() {
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
        Init();
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
                Init();
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
