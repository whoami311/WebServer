#include <unistd.h>

#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <string>

template <class T>
class BlockQueue {
public:
    explicit BlockQueue(int capacity = 1000) : m_capacity(capacity) {
        m_close = false;
        if (m_capacity <= 0) {
            std::cout << "BlockQueue capacity <= 0" << std::endl;
            exit(0);
        }
    };

    ~BlockQueue() {
        clear();
    }

    void clear() {
        {
            std::lock_guard<std::mutex> locker(m_mtx);
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

    bool push_back(const T& item) {
        std::unique_lock<std::mutex> locker(m_mtx);
        if (m_deque.size() >= m_capacity) {
            std::cout << "BlockQueue is enough !" << std::endl;
            m_cond.notify_all();
            return false;
        }
        m_deque.push_back(item);
        m_cond.notify_all();
        return true;
    }

    bool pop_front(T& item) {
        std::unique_lock<std::mutex> locker(m_mtx);
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
    std::mutex m_mtx;
    std::condition_variable m_cond;

    std::deque<T> m_deque;
    int m_capacity;
    bool m_close;
};
