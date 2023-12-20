#pragma once

#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <thread>
#include <vector>

#include "block_queue.h"
#include "common/singleton.h"

#define LOG_BASE(level, format, ...)                    \
    do {                                                \
        printf(format, ##__VA_ARGS__);                  \
        printf("\n");                                   \
        auto log = Log::Instance();                     \
        log->Write2Queue(level, format, ##__VA_ARGS__); \
        log->flush();                                   \
    } while (0);

#define LOG_DEBUG(format, ...) LOG_BASE(LOG_LEVEL::DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) LOG_BASE(LOG_LEVEL::INFO, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) LOG_BASE(LOG_LEVEL::WARN, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG_BASE(LOG_LEVEL::ERROR, format, ##__VA_ARGS__)

const constexpr char* DEFAULT_LOG_FILE_NAME = "webserver.log";

enum class LOG_LEVEL {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

class Log : public Singleton<Log, true> {
public:
    friend Singleton<Log, true>;

    ~Log() override;

    void FlushLogThread() {
        Queue2File();
    }

    void flush();
    void Write2Queue(LOG_LEVEL level, const char* format, ...);

private:
    explicit Log(const std::string& path = DEFAULT_LOG_FILE_NAME);
    void Queue2File();

    std::vector<std::string> LEVEL_STRING;
    std::string m_dirName;
    std::string m_fileName;
    int m_today;
    int m_lineCnt;
    FILE* m_fp;
    std::unique_ptr<BlockQueue<std::string>> m_queue;
    std::thread m_writeThread;
    std::mutex m_mtx;
};
