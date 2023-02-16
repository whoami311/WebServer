#ifndef LOG_H
#define LOG_H

#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdarg.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "block_queue.h"

#define DEFAULT_LOG_FILE_NAME "webserver.log"

enum E_LogLevel {
    LEVEL_DEBUG = 0,
    LEVEL_INFO = 1,
    LEVEL_WARN = 2,
    LEVEL_ERROR = 3
};

using namespace std;

class Log {
public:
    static Log* GetInstance() {
        static Log instance;
        return &instance;
    }

    static void FlushLogThread () {
        GetInstance()->Queue2File();
    }

    bool Init(string path);
    void flush();
    void Write2Queue(E_LogLevel level, const char* format, ...);
private:
    Log();
    virtual ~Log();
    void Queue2File();

    const int MAX_LINES = 10000;
    vector<string> LEVEL_STRING;
    string m_dirName;
    string m_fileName;
    string m_filePath;
    int m_today;
    int m_lineCnt;
    FILE* m_fp;
    unique_ptr<BlockQueue<string>> m_queue;
    thread m_writeThread;
    mutex m_mtx;
};

#define LOG_BASE(level, format, ...) \
    do { \
        printf(format, ##__VA_ARGS__); \
        printf("\n"); \
        Log* log = Log::GetInstance(); \
        log->Write2Queue(level, format, ##__VA_ARGS__); \
        log->flush(); \
    } while (0);

#define LOG_DEBUG(format, ...) LOG_BASE(LEVEL_DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) LOG_BASE(LEVEL_INFO, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) LOG_BASE(LEVEL_WARN, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG_BASE(LEVEL_ERROR, format, ##__VA_ARGS__)

#endif