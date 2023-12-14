#pragma once

#include <arpa/inet.h>

#include <string>
#include <unordered_map>

#include "client.h"
#include "timer.h"

class NetworkConn {
public:
    NetworkConn();
    ~NetworkConn();

    void SetGraceOpt(int fd);
    void SetReUseAddr(int fd);
    void SocketBind(int fd, const std::string& ip, int port);
    void SocketListen(int fd);

    void EpollAddFd(int fd, bool oneShot);
    void EpollModFd(int fd, int ev);
    void EpollDelFd(int fd);
    void EpollInit();

    void InitSocket(const std::string& ip, int port);

    void CloseCli(int fd);
    void AddCli(int fd, const sockaddr_in& addr);
    void AcceptClient();
    void DealErrCli(int fd);
    void DealReadCli(int fd);
    void EventLoop();

    void Close();

private:
    int m_listenFd;
    int m_epollFd;
    std::unordered_map<int, Client> m_clients;
    int m_clientCnt;
    TimerManager timer;
    bool m_running;
};
