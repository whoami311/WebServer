#include "server.h"

#include <sys/stat.h>
#include <unistd.h>

#include <iostream>

#include "log.h"
#include "thread_pool.h"

// Server::Server(const struct ConfigPara para) : m_threadPool(ThreadPool(para.threadNum, 100)) {
Server::Server(const struct ConfigPara& para) {
    Log::GetInstance()->Init(para.logPath);
    LOG_INFO("test LOG_INFO");
    ThreadPool::GetInstance()->Init(para.threadNum);

    // m_threadPool = new ThreadPool(para.threadNum, 100);
    threadNum = para.threadNum;
    m_networkConn.InitSocket("", para.port);
}

Server::~Server() {}

void Server::Init() {
    // m_networkConn.InitSocket();
}

void Server::Run() {
    m_networkConn.EventLoop();
}
