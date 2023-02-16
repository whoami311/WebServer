#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include "log.h"
#include "network_conn.h"
#include "thread_pool.h"

struct ConfigPara {
    int port;
    int threadNum;
    string logPath;
};

using namespace std;

class Server
{
public:
    Server(const struct ConfigPara para);
    ~Server();

    void Init();
    void Run();
private:
    NetworkConn m_networkConn;
    // shared_ptr<ThreadPool> m_threadPool;
    int threadNum;
};
