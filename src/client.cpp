#include "client.h"

#include <unistd.h>

#include "log.h"
#include "utils.h"

Client::Client() {}

Client::Client(int fd, const sockaddr_in& addr) : m_fd(fd), m_addr(addr) {}

Client::~Client() {}

void Client::Close() {
    if (m_fd)
        close(m_fd);
}

HTTP_RESULT Client::Process() {
    HTTP_CODE readRet = http.m_req.ProcessRead();
    if (readRet == HTTP_CODE::GET_REQUEST) {
        readRet = http.DoReq();
    }
    if (readRet == HTTP_CODE::NO_REQUEST) {
        return HTTP_RESULT::READ_FAIL;
    }

    HttpConfig config = {0};
    config.url = http.m_req.m_url;
    config.linger = http.m_req.m_linger;
    config.version = http.m_req.m_version;
    config.reqRes = readRet;

    bool writeRet = http.m_rsp.ProcessWrite(config);
    if (!writeRet) {
        return HTTP_RESULT::WRITE_FAIL;
    }
    // http.m_rsp.m_builder.Show();
    send(m_fd, http.m_rsp.m_builder.GetContent().c_str(), http.m_rsp.m_builder.GetContent().size(), 0);
    // send(m_fd, http.m_rsp.m_fileAddr, http.m_rsp.m_fileStat.st_size, 0);
    send(m_fd, http.m_rsp.fileContent.c_str(), http.m_rsp.m_fileStat.st_size, 0);

    http.m_req.Reset();
    return HTTP_RESULT::SUCCEED;
}

HTTP_RESULT Client::DealRead() {
    if (!http.m_req.ReadOnce(m_fd)) {
        LOG_WARN("ReadOnce failed");
        return HTTP_RESULT::READ_FAIL;
    }
    return Process();
}

void Client::DealWrite() {
    // Write();
}

std::weak_ptr<TimerNode> Client::GetTimerNode() {
    return timerNode;
}

void Client::SetTimerNode(std::shared_ptr<TimerNode> node) {
    timerNode = node;
}

void Client::DelTimer() {
    if (timerNode.lock()) {
        std::shared_ptr<TimerNode> tempTimer(timerNode.lock());
        // tempTimer->Abandon();
        tempTimer->deleted = true;
        timerNode.reset();
    }
}
