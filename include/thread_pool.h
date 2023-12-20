#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

#include "common/singleton.h"

constexpr int DEFAULT_TASK_CAPACITY = 100;

class ThreadPool : public Singleton<ThreadPool, true> {
public:
    friend Singleton<ThreadPool, true>;

    ~ThreadPool() override;

    bool AppendTask(std::function<void()> task);

private:
    ThreadPool(int threadNum = 4, int taskCapacity = DEFAULT_TASK_CAPACITY);

    int m_threadNum;
    int m_taskCapacity;

    std::mutex m_mtx;
    std::condition_variable m_cond;
    std::queue<std::function<void()>> m_taskQueue;
    bool m_close;
};
