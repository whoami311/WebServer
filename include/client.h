#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>
#include "utils.h"
#include "timer.h"
#include "http/http_data.h"

using namespace std;

enum E_HTTP_RESULT {
    HTTP_RESULT_SUCCEED = 0,
    HTTP_RESULT_READ_FAIL = 1,
    HTTP_RESULT_WRITE_FAIL = 2
};

class Client {
public:
    // Client(const int fd, sockaddr_in addr);
    Client();
    ~Client();

    void init(const int fd, sockaddr_in addr);
    void Close();
    E_HTTP_RESULT Process();
    E_HTTP_RESULT DealRead();
    void DealWrite();
    weak_ptr<TimerNode> GetTimerNode();
    void SetTimerNode(shared_ptr<TimerNode> node);
    void DelTimer();
private:
    int m_fd;
    sockaddr_in m_addr;
    HttpData http;
    weak_ptr<TimerNode> timerNode;
};

#endif
