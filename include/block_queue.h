#include <unistd.h>
#include <iostream>
#include <string>
#include <deque>
#include <mutex>
#include <condition_variable>

using namespace std;

template<class T>
class BlockQueue
{
public:
    explicit BlockQueue(int capacity = 1000) : m_capacity(capacity) {
        m_close = false;
        if (m_capacity <= 0) {
            cout << "BlockQueue capacity <= 0" << endl;
            exit(0);
        }
    };

    ~BlockQueue() {
        clear();
    }

    void clear() {
        {
            lock_guard<mutex> locker(m_mtx);
            m_deque.clear();
        }
        m_cond.notify_all();
    }

    void close() {
        m_close = true;
        clear();
    }

    bool empty() {
        return m_deque.empty();
    }

    int capacity() {
        return m_capacity;
    }

    int size() {
        return m_deque.size();
    }

    T front() {
        return m_deque.front();
    }

    T back() {
        return m_deque.back();
    }

    void flush() {
        m_cond.notify_all();
    }

    bool push_back(const T &item) {
        unique_lock<mutex> locker(m_mtx);
        if (m_deque.size() >= m_capacity) {
            cout << "BlockQueue is enough !" << endl;
            m_cond.notify_all();
            return false;
        }
        m_deque.push_back(item);
        m_cond.notify_all();
        return true;
    }

    bool pop_front(T &item) {
        unique_lock<mutex> locker(m_mtx);
        while (m_deque.empty()) {
            if (m_close)
                return false;
            m_cond.wait(locker);
        }
        item = m_deque.front();
        m_deque.pop_front();
        return true;
    }

private:
    mutex m_mtx;
    condition_variable m_cond;

    deque<T> m_deque;
    int m_capacity;
    bool m_close;
};
