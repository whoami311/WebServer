#include "server.h"

// Server::Server(const struct ConfigPara para) : m_threadPool(ThreadPool(para.threadNum, 100)) {
Server::Server(const struct ConfigPara para) {
    Log::GetInstance()->Init(para.logPath);
    LOG_INFO("test LOG_INFO");
    ThreadPool::GetInstance()->Init(para.threadNum, DEFAULT_TASK_CAPACITY);

    // m_threadPool = new ThreadPool(para.threadNum, 100);
    threadNum = para.threadNum;
    m_networkConn.InitSocket("", para.port);
}

Server::~Server() {}

void Server::Init() {
    // m_networkConn.InitSocket();
}

void Server::Run() {
    // ThreadPool pool(threadNum, 100);
    m_networkConn.EventLoop();
}
