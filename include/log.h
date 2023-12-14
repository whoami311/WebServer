#pragma once

#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <thread>
#include <vector>

#include "block_queue.h"

enum class LOG_LEVEL {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

class Log {
public:
    static Log* GetInstance() {
        static Log instance;
        return &instance;
    }

    static void FlushLogThread() {
        GetInstance()->Queue2File();
    }

    bool Init(const std::string& path);
    void flush();
    void Write2Queue(LOG_LEVEL level, const char* format, ...);

private:
    Log();
    ~Log();
    void Queue2File();

    std::vector<std::string> LEVEL_STRING;
    std::string m_dirName;
    std::string m_fileName;
    std::string m_filePath;
    int m_today;
    int m_lineCnt;
    FILE* m_fp;
    std::unique_ptr<BlockQueue<std::string>> m_queue;
    std::thread m_writeThread;
    std::mutex m_mtx;
};

#define LOG_BASE(level, format, ...)                    \
    do {                                                \
        printf(format, ##__VA_ARGS__);                  \
        printf("\n");                                   \
        Log* log = Log::GetInstance();                  \
        log->Write2Queue(level, format, ##__VA_ARGS__); \
        log->flush();                                   \
    } while (0);

#define LOG_DEBUG(format, ...) LOG_BASE(LOG_LEVEL::DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) LOG_BASE(LOG_LEVEL::INFO, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) LOG_BASE(LOG_LEVEL::WARN, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG_BASE(LOG_LEVEL::ERROR, format, ##__VA_ARGS__)
