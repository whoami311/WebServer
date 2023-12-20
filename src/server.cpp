#include "server.h"

#include <sys/stat.h>
#include <unistd.h>

#include <iostream>

#include "log.h"
#include "thread_pool.h"

Server::Server(const struct ConfigPara& para) {
    Log::Create(para.logPath);
    LOG_INFO("test LOG_INFO");
    ThreadPool::Create(para.threadNum);

    threadNum = para.threadNum;
    m_networkConn.InitSocket("", para.port);
}

Server::~Server() {
    ThreadPool::Destroy();
    Log::Destroy();
}

void Server::Run() {
    m_networkConn.EventLoop();
}
