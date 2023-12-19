#include <string>

#include "network_conn.h"

struct ConfigPara {
    int port;
    int threadNum;
    std::string logPath;
};

class Server {
public:
    Server(const struct ConfigPara& para);
    ~Server();

    void Run();

private:
    NetworkConn m_networkConn;
    // shared_ptr<ThreadPool> m_threadPool;
    int threadNum;
};
