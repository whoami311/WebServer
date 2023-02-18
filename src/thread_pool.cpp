#include <thread_pool.h>

using namespace std;

ThreadPool::ThreadPool() {
    m_close = false;
}

void ThreadPool::Init(int threadNum = 4, int taskCapacity = DEFAULT_TASK_CAPACITY)
{
    CORRECT_NUMBER_RANGE(threadNum, 1, MAX_THERAD_NUM);
    CORRECT_NUMBER_RANGE(taskCapacity, 1, MAX_TASK_CAPACITY);
    m_threadNum = threadNum;
    m_taskCapacity = taskCapacity;
    for (int i=0; i<m_threadNum; i++) {
        thread([pool = this] {
            string name("thread_in_pool");
            prctl(PR_SET_NAME, name.c_str());
            unique_lock<mutex> locker(pool->m_mtx);
            while (!pool->m_close) {
                if (pool->m_taskQueue.empty())
                    pool->m_cond.wait(locker);
                else {
                    LOG_INFO("will run task in ThreadPool");
                    function<void()> task = pool->m_taskQueue.front();
                    task();
                    pool->m_taskQueue.pop();
                }
            }
            
        }).detach();
    }
}

ThreadPool::~ThreadPool()
{
    m_close = true;
    while (!m_taskQueue.empty()) {
        m_taskQueue.pop();
    }
    m_cond.notify_all();
}

bool ThreadPool::AppendTask(function<void()> task) {
    if (m_taskQueue.size() < m_taskCapacity) {
        m_taskQueue.emplace(task);
        m_cond.notify_one();
        return true;
    } else {
        return false;
    }
}