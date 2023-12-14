#include <thread_pool.h>

#include <sys/prctl.h>

#include <thread>

#include "log.h"
#include "utils.h"

constexpr int MAX_THERAD_NUM = 1024;
constexpr int MAX_TASK_CAPACITY = 10000;

ThreadPool::ThreadPool() {
    m_close = false;
}

void ThreadPool::Init(int threadNum, int taskCapacity) {
    utils::CorrectNumberRange(threadNum, 1, MAX_THERAD_NUM);
    utils::CorrectNumberRange(taskCapacity, 1, MAX_TASK_CAPACITY);
    m_threadNum = threadNum;
    m_taskCapacity = taskCapacity;
    for (int i = 0; i < m_threadNum; i++) {
        std::thread([pool = this] {
            std::string name("thread_in_pool");
            prctl(PR_SET_NAME, name.c_str());
            std::unique_lock<std::mutex> locker(pool->m_mtx);
            while (!pool->m_close) {
                if (pool->m_taskQueue.empty())
                    pool->m_cond.wait(locker);
                else {
                    LOG_INFO("will run task in ThreadPool");
                    std::function<void()> task = pool->m_taskQueue.front();
                    task();
                    pool->m_taskQueue.pop();
                }
            }
        }).detach();
    }
}

ThreadPool::~ThreadPool() {
    m_close = true;
    while (!m_taskQueue.empty()) {
        m_taskQueue.pop();
    }
    m_cond.notify_all();
}

bool ThreadPool::AppendTask(std::function<void()> task) {
    if (m_taskQueue.size() < m_taskCapacity) {
        m_taskQueue.emplace(std::move(task));
        m_cond.notify_one();
        return true;
    } else {
        return false;
    }
}