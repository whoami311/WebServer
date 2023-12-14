#include "network_conn.h"

#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <vector>

#include "log.h"
#include "thread_pool.h"
#include "utils.h"

constexpr int GRACE_CLOSE_TIME = 3;
constexpr int MAX_PORT = 65535;
constexpr int MIN_PORT = 1;
constexpr int DEFAULT_PORT = 80;
constexpr int SOCKET_LISTEN_NUM = 1024;
constexpr int CLOSE_SOCKET_MS = 20 * 1000;
constexpr int MAX_EVENT_NUM = 10000;
constexpr int MAX_CLIENT_NUM = 10000;
// -1 block
constexpr int EPOLL_WAIT_TIMEOUT = -1;

NetworkConn::NetworkConn() {
    m_running = true;
}

NetworkConn::~NetworkConn() {
    Close();
}

void NetworkConn::SetGraceOpt(int fd) {
    struct linger opt = {0};
    opt.l_onoff = 1;
    opt.l_linger = GRACE_CLOSE_TIME;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt));
}

void NetworkConn::SetReUseAddr(int fd) {
    int reUseAddr = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&reUseAddr, sizeof(reUseAddr));
}

void NetworkConn::SocketBind(int fd, const std::string& ip, int port) {
    if (port == 0)
        port = DEFAULT_PORT;
    utils::CorrectNumberRange(port, MIN_PORT, MAX_PORT);
    LOG_INFO("port == %d", port);

    struct sockaddr_in addr = {0};
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    if (ip.empty())
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
    addr.sin_port = htons(port);

    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        LOG_ERROR("bind failed!, return %d, err: %s", ret, strerror(errno));
        close(m_listenFd);
        exit(0);
    }
}

void NetworkConn::SocketListen(int fd) {
    int ret = listen(fd, SOCKET_LISTEN_NUM);
    if (ret < 0) {
        LOG_ERROR("listen failed!, return %d", ret);
        close(m_listenFd);
        exit(0);
    }
}

void NetworkConn::InitSocket(const std::string& ip, int port) {
    m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenFd < 0) {
        LOG_ERROR("socket build failed");
        exit(0);
    }

    SetGraceOpt(m_listenFd);
    SetReUseAddr(m_listenFd);
    SocketBind(m_listenFd, ip, port);
    SocketListen(m_listenFd);
    utils::SetNonBlock(m_listenFd);

    EpollInit();
}

void NetworkConn::EpollAddFd(int fd, bool oneShot) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if (oneShot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &event);
    return;
}

void NetworkConn::EpollModFd(int fd, int ev) {
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLRDHUP | EPOLLONESHOT;
    // event.events = ev | EPOLLET | EPOLLRDHUP;
    epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &event);
}

void NetworkConn::EpollDelFd(int fd) {
    epoll_event event = {0};
    epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &event);
}

void NetworkConn::EpollInit() {
    m_epollFd = epoll_create1(EPOLL_CLOEXEC);
    if (m_epollFd < 0) {
        LOG_ERROR("epoll_create failed!, return %d", m_epollFd);
        exit(0);
    }

    EpollAddFd(m_listenFd, false);
}

void NetworkConn::CloseCli(int fd) {
    EpollDelFd(fd);
    m_clients[fd].Close();
    m_clients.erase(fd);
    m_clientCnt--;
    LOG_INFO("CloseCli fd == %d", fd);
    LOG_INFO("after CloseCli, m_clients.size() == %ld", m_clients.size());
}

void NetworkConn::AddCli(int fd, const sockaddr_in& addr) {
    // Client client(fd, addr);
    // m_clients[fd] = client;
    m_clients[fd].Init(fd, addr);
    m_clientCnt++;
    EpollAddFd(fd, true);
    // m_clients[fd].SetTimerNode(timer.AddTimer(std::bind(&NetworkConn::CloseCli, this, fd), CLOSE_SOCKET_MS));
    std::shared_ptr<TimerNode> node = timer.AddTimer(std::bind(&NetworkConn::CloseCli, this, fd), CLOSE_SOCKET_MS);
    m_clients[fd].SetTimerNode(node);
    utils::SetNonBlock(fd);
}

void NetworkConn::AcceptClient() {
    struct sockaddr_in cliAddr = {0};
    socklen_t addrLen = sizeof(cliAddr);

    int connFd = accept(m_listenFd, (struct sockaddr*)&cliAddr, &addrLen);
    LOG_INFO("connFd = %d", connFd);
    if (connFd < 0) {
        if (errno == EINVAL)
            LOG_WARN("accept errno EINVAL");
        LOG_WARN("accept failed!, return %d, errno = %d", connFd, errno);
        return;
    }
    if (m_clientCnt > MAX_CLIENT_NUM) {
        LOG_WARN("user number is full!");
        return;
    }
    AddCli(connFd, cliAddr);
}

void NetworkConn::DealErrCli(int fd) {
    m_clients[fd].DelTimer();
    CloseCli(fd);
}

void NetworkConn::DealReadCli(int fd) {
    HTTP_RESULT res = m_clients[fd].DealRead();
    switch (res) {
        case HTTP_RESULT::SUCCEED:
            // EpollModFd(fd, EPOLLOUT);
            // break;

        case HTTP_RESULT::READ_FAIL:
            EpollModFd(fd, EPOLLIN);
            break;

        case HTTP_RESULT::WRITE_FAIL:
            DealErrCli(fd);
            break;

        default:
            break;
    }
}

void NetworkConn::EventLoop() {
    std::vector<epoll_event> events(MAX_EVENT_NUM);

    while (m_running) {
        std::cout << "will epoll_wait, events.size() == " << events.size() << std::endl;
        // EpollAddFd(m_listenFd);
        int eventCnt = epoll_wait(m_epollFd, &events[0], events.size(), EPOLL_WAIT_TIMEOUT);
        if (eventCnt < 0 && errno != EINTR) {
            LOG_ERROR("epoll_wait failed!, return %d, errno = %d", eventCnt, errno);
            exit(0);
        }
        LOG_INFO("eventCnt == %d", eventCnt);

        for (int i = 0; i < eventCnt; i++) {
            epoll_event event = events[i];
            int fd = event.data.fd;
            if (fd == m_listenFd) {
                LOG_INFO("will AcceptClient");
                AcceptClient();
            } else if (event.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                LOG_INFO("will DealErrCli, fd == %d", fd);
                LOG_INFO("events == %x, maybe %x, %x, %x", event.events, EPOLLRDHUP, EPOLLHUP, EPOLLERR);
                // EPOLLRDHUP 对端断开连接
                DealErrCli(fd);
            } else if (event.events & (EPOLLIN | EPOLLPRI)) {
                LOG_INFO("will DealReadCli, fd == %d", fd);
                // DealReadCli(fd);
                ThreadPool::GetInstance()->AppendTask(std::bind(&NetworkConn::DealReadCli, this, fd));
            } else if (event.events & EPOLLOUT) {
                LOG_INFO("will DealWrite, fd == %d", fd);
                m_clients[fd].DealWrite();
            } else {
                LOG_INFO("some other things happened.");
            }
        }
        timer.HandleTimer();
    }
}

void NetworkConn::Close() {
    m_running = false;
    if (m_listenFd)
        close(m_listenFd);
    if (m_epollFd)
        close(m_epollFd);
}
