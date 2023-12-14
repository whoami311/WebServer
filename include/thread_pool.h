#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

constexpr int DEFAULT_TASK_CAPACITY = 100;

// class ThreadPool
// {
// public:
//     ThreadPool(int threadNum, int taskCapacity);
//     ~ThreadPool();

//     bool AppendTask(function<void()> task);
// private:
//     int m_threadNum;
//     int m_taskCapacity;

//     mutex m_mtx;
//     condition_variable m_cond;
//     queue<function<void()>> m_taskQueue;
//     bool m_close;
// };

class ThreadPool {
public:
    static ThreadPool* GetInstance() {
        static ThreadPool instance;
        return &instance;
    }
    void Init(int threadNum = 4, int taskCapacity = DEFAULT_TASK_CAPACITY);
    bool AppendTask(std::function<void()> task);

private:
    ThreadPool();
    ~ThreadPool();

    int m_threadNum;
    int m_taskCapacity;

    std::mutex m_mtx;
    std::condition_variable m_cond;
    std::queue<std::function<void()>> m_taskQueue;
    bool m_close;
};
