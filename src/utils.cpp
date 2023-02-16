#include "utils.h"

using namespace std;

void SetNonBlock(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
    return;
}

string GetFileSuffix (string file) {
    int index = file.find_last_of('.');
    if (index == 0)
        return "";
    return file.substr(index + 1);
}
