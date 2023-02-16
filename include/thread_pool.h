#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <sys/prctl.h>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
#include "log.h"
#include "utils.h"

#define MAX_THERAD_NUM (1024)
#define MAX_TASK_CAPACITY (10000)
#define DEFAULT_TASK_CAPACITY (100)

using namespace std;

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

class ThreadPool
{
public:
    static ThreadPool* GetInstance() {
        static ThreadPool instance;
        return &instance;
    }
    void Init(int threadNum, int taskCapacity);
    bool AppendTask(function<void()> task);
private:
    ThreadPool();
    ~ThreadPool();

    int m_threadNum;
    int m_taskCapacity;

    mutex m_mtx;
    condition_variable m_cond;
    queue<function<void()>> m_taskQueue;
    bool m_close;
};

#endif
