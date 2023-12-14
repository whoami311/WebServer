#include <string>

namespace utils {

template <typename T>
void CorrectNumberRange(T& x, T mi, T ma);

void SetNonBlock(int fd);

std::string GetFileSuffix(const std::string& file);

}  // namespace utils
