#include <string>
#include <fcntl.h>

using namespace std;

#define CORRECT_NUMBER_RANGE(x, mi, ma) \
    do { \
        x = min(x, ma); \
        x = max(x, mi); \
    } while (0);

void SetNonBlock(int fd);

string GetFileSuffix (string file);
