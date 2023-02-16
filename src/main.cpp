#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include "server.h"

#define DEFAULT_SERVER_PORT 80
#define DEFAULT_THREAD_NUM 4

using namespace std;

bool CheckPath(string path) {
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == -1) {
        cout << "invalid path" << endl;
        return false;
    }
    if (!S_ISDIR(fileStat.st_mode) || access(path.c_str(), R_OK) == -1) {
        return false;
    }
    return true;
}

void SetDefaultPara(struct ConfigPara* para) {
    para->port = DEFAULT_SERVER_PORT;
    para->threadNum = DEFAULT_THREAD_NUM;
    para->logPath = DEFAULT_LOG_FILE_NAME;
    return ;
}

void ParseConfigPara(int argc, char *argv[], struct ConfigPara* para) {
    SetDefaultPara(para);

    const char optStr[16] = "p::t::l::";
    int opt;
    while ((opt = getopt(argc, argv, optStr)) != -1) {
        switch (opt)
        {
            case 'p':
                para->port = atoi(optarg);
                break;

            case 't':
                para->threadNum = atoi(optarg);
                break;
            
            case 'l':
                para->logPath = optarg;
                if (!CheckPath(para->logPath)) {
                    exit(0);
                }
                break;

            case '?':
                cout << "unknown argument option." << endl;
                break;
            
            default:
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    struct ConfigPara para = {0};
    ParseConfigPara(argc, argv, &para);

    Server server(para);
    server.Run();

    sleep(5);
    cout << "webserver process end" << endl;
    return 0;
}