#include "timer.h"

#include "log.h"

// TimerNode::TimerNode(TimerCb cb, int timeout) {
//     expires = Clock::now() + MS(timeout);
//     cb = cb;
//     deleted = false;
// }

// TimerNode::~TimerNode() {}

// void TimerNode::AdjustExpires(int timeout) {
//     expires = expires + MS(timeout);
// }

// void TimerNode::Abandon() {
//     deleted = false;
// }

// TimePoint TimerNode::GetExpires() {
//     return expires;
// }

// TimerCb TimerNode::GetCb() {
//     return cb;
// }

// bool TimerNode::IsDeleted() {
//     return deleted;
// }

TimerManager::TimerManager() {}

TimerManager::~TimerManager() {
    clear();
}

// TimerNodePointer TimerManager::AddTimer(TimerCb cb, int timeout) {
TimerNodePointer TimerManager::AddTimer(TimerCb cb, int timeout) {
    std::lock_guard<std::mutex> locker(m_mtx);
    TimerNode node = {0};
    node.expires = Clock::now() + MS(timeout);
    node.cb = std::move(cb);
    node.deleted = false;
    TimerNodePointer ptr = std::make_shared<TimerNode>(node);
    m_heap.push(ptr);
    return ptr;
    // TimerNodePointer ptr(new TimerNode(cb, timeout));
    // m_heap.push(ptr);
    // return ptr;
}

// bool TimerManager::AddTimer(TimerNode node) {
//     m_heap.push(make_shared<TimerNode>(node));
//     return true;
// }

void TimerManager::HandleTimer() {
    std::lock_guard<std::mutex> locker(m_mtx);
    while (!m_heap.empty()) {
        TimerNodePointer node = m_heap.top();
        LOG_INFO("m_heap.size() = %ld in HandleTimer", m_heap.size());
        if (node == nullptr) {
            LOG_INFO("node == nullptr in HandleTimer");
            m_heap.pop();
            continue;
        }
        // if (node->IsDeleted()) {
        if (node->deleted) {
            LOG_INFO("node is deleted in HandleTimer");
            m_heap.pop();
            continue;
        }
        // if (chrono::duration_cast<MS>(node->GetExpires() - Clock::now()).count() > 0)
        if (std::chrono::duration_cast<MS>(node->expires - Clock::now()).count() > 0) {
            LOG_INFO("node untimed in HandleTimer");
            break;
        }
        LOG_INFO("node will run in HandleTimer");
        // TimerCb cb = node->GetCb();
        node->cb();
        m_heap.pop();
    }
}

void TimerManager::clear() {
    while (!m_heap.empty()) {
        m_heap.pop();
    }
}
