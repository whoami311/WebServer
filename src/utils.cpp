#include "utils.h"

#include <fcntl.h>

namespace utils {

template <typename T>
void CorrectNumberRange(T& x, T mi, T ma) {
    x = std::min(x, ma);
    x = std::max(x, mi);
}

template void CorrectNumberRange(int& x, int mi, int ma);

void SetNonBlock(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
    return;
}

std::string GetFileSuffix(const std::string& file) {
    int index = file.find_last_of('.');
    if (index == 0)
        return "";
    return file.substr(index + 1);
}

}
