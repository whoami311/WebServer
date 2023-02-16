#ifndef TIMER_H
#define TIMER_H

#include <mutex>
#include <chrono>
#include <queue>
#include <functional>
#include <memory>
#include "log.h"

using namespace std;

struct TimerNode;
// class TimerNode;

typedef std::function<void()> TimerCb;
typedef std::chrono::high_resolution_clock Clock;
typedef Clock::time_point TimePoint;
typedef std::shared_ptr<TimerNode> TimerNodePointer;
typedef std::chrono::milliseconds MS;

// class TimerNode {
// public:
//     TimerNode(TimerCb cb, int timeout);
//     ~TimerNode();
//     void AdjustExpires(int timeout);
//     void Abandon();
//     TimePoint GetExpires();
//     TimerCb GetCb();
//     bool IsDeleted();

//     bool operator<(const TimerNode &t) {
//         return expires < t.expires;
//     }

//     TimerCb cb;

// private:
//     TimePoint expires;
//     bool deleted;
// };

struct TimerNode {
    TimerCb cb;
    TimePoint expires;
    bool deleted;

    bool operator<(const TimerNode &t) {
        return expires < t.expires;
    }
};

struct TimerCmp {
    bool operator()(const TimerNodePointer t1, const TimerNodePointer t2) {
        // return t1->GetExpires() > t2->GetExpires();
        return t1->expires > t2->expires;
    }
};

class TimerManager {
public:
    TimerManager();
    ~TimerManager();

    // TimerNodePointer AddTimer(TimerCb cb, int timeout);
    TimerNodePointer AddTimer(const TimerCb& cb, int timeout);
    // bool AddTimer(TimerNode node);
    // bool DelTimer();
    void HandleTimer();
    void clear();
private:
    priority_queue<TimerNodePointer, deque<TimerNodePointer>, TimerCmp> m_heap;
    mutex m_mtx;
};

#endif