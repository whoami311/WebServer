#include "client.h"

using namespace std;

Client::Client() {

}

// Client::Client(cosnt int fd, sockaddr_in addr) {
//     m_fd = fd;
//     m_addr = addr;
// }

void Client::init(const int fd, sockaddr_in addr) {
    m_fd = fd;
    m_addr = addr;
}

Client::~Client() {}

void Client::Close() {
    close(m_fd);
}

E_HTTP_RESULT Client::Process() {
    E_HTTP_CODE readRet = http.m_req.ProcessRead();
    if (readRet == GET_REQUEST) {
        readRet = http.DoReq();
    }
    if (readRet == NO_REQUEST) {
        return HTTP_RESULT_READ_FAIL;
    }

    HttpConfig config = {0};
    config.url = http.m_req.m_url;
    config.linger = http.m_req.m_linger;
    config.version = http.m_req.m_version;
    config.reqRes = readRet;

    bool writeRet = http.m_rsp.ProcessWrite(config);
    if (!writeRet) {
        return HTTP_RESULT_WRITE_FAIL;
    }
    // http.m_rsp.m_builder.show();
    send(m_fd, http.m_rsp.m_builder.getContent().c_str(), http.m_rsp.m_builder.getContent().size(), 0);
    // send(m_fd, http.m_rsp.m_fileAddr, http.m_rsp.m_fileStat.st_size, 0);
    send(m_fd, http.m_rsp.fileContent.c_str(), http.m_rsp.m_fileStat.st_size, 0);

    http.m_req.Reset();
    return HTTP_RESULT_SUCCEED;
}

E_HTTP_RESULT Client::DealRead() {
    if (!http.m_req.ReadOnce(m_fd)) {
        LOG_WARN("ReadOnce failed");
        return HTTP_RESULT_READ_FAIL;
    }
    return Process();
}

void Client::DealWrite() {
    // Write();
}

weak_ptr<TimerNode> Client::GetTimerNode() {
    return timerNode;
}

void Client::SetTimerNode(shared_ptr<TimerNode> node) {
    timerNode = node;
}

void Client::DelTimer() {
    if (timerNode.lock()) {
        shared_ptr<TimerNode> tempTimer(timerNode.lock());
        // tempTimer->Abandon();
        tempTimer->deleted = true;
        timerNode.reset();
    }
}
