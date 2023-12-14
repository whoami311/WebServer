#include "http/http_data.h"

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "log.h"

void HttpData::Init() {
    m_realFile = "";
}

HTTP_CODE HttpData::DoReq() {
    if (m_req.m_url == "/")
        m_realFile = "../pages/index.html";
    else
        m_realFile = "../pages" + m_req.m_url;

    if (stat(m_realFile.c_str(), &m_rsp.m_fileStat) < 0)
        return HTTP_CODE::NO_RESOURCE;

    if (!(m_rsp.m_fileStat.st_mode & S_IROTH))
        return HTTP_CODE::FORBIDDEN_REQUEST;

    if (S_ISDIR(m_rsp.m_fileStat.st_mode))
        return HTTP_CODE::BAD_REQUEST;

    int fd = open(m_realFile.c_str(), O_RDONLY);
    if (fd < 0) {
        LOG_WARN("open %s failed", m_realFile.c_str());
        return HTTP_CODE::NO_RESOURCE;
    }
    // LOG_INFO("fd == %d in DoReq", fd);
    m_rsp.m_fileAddr = (char *)mmap(0, m_rsp.m_fileStat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    m_rsp.fileContent = std::string(m_rsp.m_fileAddr, m_rsp.m_fileStat.st_size);
    m_rsp.unmap();

    return HTTP_CODE::FILE_REQUEST;
}
