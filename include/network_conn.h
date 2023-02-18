#ifndef NETWORK_CONN_H
#define NETWORK_CONN_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "log.h"
#include "utils.h"
#include "client.h"
#include "timer.h"
#include "thread_pool.h"

#define GRACE_CLOSE_TIME (3)
#define MAX_PORT (65535)
#define MIN_PORT (1)
#define DEFAULT_PORT (80)
#define SOCKET_LISTEN_NUM (1024)
#define CLOSE_SOCKET_MS (20 * 1000)
#define MAX_EVENT_NUM (10000)
#define MAX_CLIENT_NUM (10000)
// -1 block
#define EPOLL_WAIT_TIMEOUT (-1)

using namespace std;

class NetworkConn {
public:
    NetworkConn();
    ~NetworkConn();

    void SetGraceOpt(int fd);
    void SetReUseAddr(int fd);
    void SocketBind(int fd, string ip, int port);
    void SocketListen(int fd);

    void EpollAddFd(int fd, bool oneShot);
    void EpollModFd(int fd, int ev);
    void EpollDelFd(int fd);
    void EpollInit();

    void InitSocket(string ip, int port);

    void CloseCli(int fd);
    void AddCli(int fd, sockaddr_in addr);
    void AcceptClient();
    void DealErrCli(int fd);
    void DealReadCli(int fd);
    void EventLoop();

    void Close();
private:
    int m_listenFd;
    int m_epollFd;
    unordered_map<int, Client> m_clients;
    int m_clientCnt;
    TimerManager timer;
    bool m_running;
};

#endif