#include "log.h"

using namespace std;

Log::Log() {
    m_lineCnt = 0;
    m_fp = nullptr;
    m_queue = nullptr;
}

Log::~Log() {
    cout << "~Log start." << endl;
    if (m_writeThread.joinable()) {
        while (!m_queue->empty()) {
            m_queue->flush();
        }
        m_queue->close();
        cout << "~Log will join." << endl;
        m_writeThread.join();
    }
    cout << "~Log end." << endl;
    if (m_fp) {
        flush();
        fclose(m_fp);
    }
}

void Log::flush() {
    lock_guard<mutex> locker(m_mtx);
    m_queue->flush();
    fflush(m_fp);
}

bool Log::Init(string path) {
    // LEVEL_STRING = ["[debug]:", "[info]:", "[warn]:", "[error]:"];
    LEVEL_STRING.push_back("[debug]:");
    LEVEL_STRING.push_back("[info]:");
    LEVEL_STRING.push_back("[warn]:");
    LEVEL_STRING.push_back("[error]:");
    if (!m_queue) {
        m_queue = make_unique<BlockQueue<string>> ();
        m_writeThread = thread(FlushLogThread);
    }

    int pos = path.find_last_of('/');
    if (pos == string::npos) {
        m_dirName = ".";
        m_fileName = path;
    } else if (pos == path.size() - 1) {
        m_dirName = path.substr(0, pos);
        m_fileName = DEFAULT_LOG_FILE_NAME;
    } else {
        m_dirName = path.substr(0, pos);
        m_fileName = path.substr(pos+1);
    }

    time_t t = time(nullptr);
    struct tm* sysTime = localtime(&t);

    m_today = sysTime->tm_mday;
    // m_filePath = m_dirName + '/' + m_fileName;
    char filePath[256] = {};
    snprintf(filePath, sizeof(filePath)-1, "%s/%04d_%02d_%02d_%s", m_dirName.c_str(), sysTime->tm_year + 1900,
        sysTime->tm_mon + 1, sysTime->tm_mday, m_fileName.c_str());

    if (m_fp) {
        flush();
        fclose(m_fp);
    }

    m_fp = fopen(filePath, "a");
    if (m_fp == nullptr) {
        cout << "mkdir  " << m_dirName << endl;
        mkdir(m_dirName.c_str(), 0777);
        m_fp = fopen(filePath, "a");
        if (m_fp == nullptr) {
            cout << "open " << filePath << " failed." << endl;
            return false;
        }
    }
    return true;
}

void Log::Queue2File() {
    string str;
    while (m_queue->pop_front(str)) {
        lock_guard<mutex> locker(m_mtx);
        fputs(str.c_str(), m_fp);
    }
}

void Log::Write2Queue(E_LogLevel level, const char* format, ...) {
    struct timeval now = {0};
    gettimeofday(&now, nullptr);
    time_t sec = now.tv_sec;
    struct tm* sysTime = localtime(&sec);

    m_lineCnt++;

    if (m_today != sysTime->tm_mday || m_lineCnt % MAX_LINES == 0) {
        char newFile[256] = {0};
        char tail[16] = {0};
        snprintf(tail, sizeof(tail)-1, "%04d_%02d_%02d", sysTime->tm_year + 1900, sysTime->tm_mon + 1, sysTime->tm_mday);
        if (m_today != sysTime->tm_mday) {
            snprintf(newFile, sizeof(newFile)-1, "%s/%s_%s", m_dirName.c_str(), tail, m_fileName.c_str());
            m_today = sysTime->tm_mday;
            m_lineCnt = 0;
        } else
            snprintf(newFile, sizeof(newFile)-1, "%s/%s_%d_%s", m_dirName.c_str(), tail, m_lineCnt / MAX_LINES,
                m_fileName.c_str());
        {
            lock_guard<mutex> locker(m_mtx);
            flush();
            fclose(m_fp);
            m_fp = fopen(newFile, "a");
        }
        if (m_fp == nullptr) {
            cout << "open " << newFile << " failed." << endl;
            exit(0);
        }
    }

    va_list vaLst;
    va_start(vaLst, format);
    char buf[1024] = {0};

    int n = snprintf(buf, sizeof(buf)-1, "%04d-%02d-%02d %02d:%02d:%02d.%06ld %s ", sysTime->tm_year + 1900,
        sysTime->tm_mon + 1, sysTime->tm_mday, sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec, now.tv_usec,
        LEVEL_STRING[level].c_str());
    int m = vsnprintf(buf + n, sizeof(buf) - n, format, vaLst);
    buf[n + m] = '\n';
    buf[n + m + 1] = '\0';
    string str = buf;
    m_queue->push_back(str);
    va_end(vaLst);

    return ;
}
