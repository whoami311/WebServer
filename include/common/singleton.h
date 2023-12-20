#pragma once

#include <memory>
#include <mutex>

template <typename T, bool lazy = false>
class Singleton {
public:
    virtual ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

    template <typename... Args>
    static std::shared_ptr<T> Create(Args&&... args) {
        if (!instance_) {
            static std::mutex mtx;
            std::lock_guard<std::mutex> lk(mtx);
            if (!instance_)
                instance_ = std::shared_ptr<T>(new T(std::forward<Args>(args)...));
        }
        return instance_;
    }

    static void Destroy() {
        instance_ = nullptr;
    }

    static std::shared_ptr<T> Instance() {
        return Create();
    }

protected:
    Singleton() = default;

private:
    static std::shared_ptr<T> instance_;
};

template <typename T, bool lazy>
std::shared_ptr<T> Singleton<T, lazy>::instance_ = lazy ? nullptr : std::shared_ptr<T>(new T);
