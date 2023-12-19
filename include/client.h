#pragma once

#include <netinet/in.h>

#include "http/http_data.h"
#include "timer.h"

enum class HTTP_RESULT {
    SUCCEED = 0,
    READ_FAIL = 1,
    WRITE_FAIL = 2
};

class Client {
public:
    Client();
    Client(int fd, const sockaddr_in& addr);
    ~Client();

    void Close();
    HTTP_RESULT Process();
    HTTP_RESULT DealRead();
    void DealWrite();
    std::weak_ptr<TimerNode> GetTimerNode();
    void SetTimerNode(std::shared_ptr<TimerNode> node);
    void DelTimer();

private:
    int m_fd;
    sockaddr_in m_addr;
    HttpData http;
    std::weak_ptr<TimerNode> timerNode;
};
