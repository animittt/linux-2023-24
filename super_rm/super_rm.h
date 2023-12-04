#ifndef SUPER_RM_H

#include <string>
#include <vector>

namespace
{

constexpr size_t blockSize = 4096;

std::vector<char> generateRandomVector();

std::size_t getFileSize(int fd);

void secureDeleteFile(const std::string &filename, bool verbose);

bool removeDirectory(const char* path, bool verbose);

} // Unnamed namespace.

#endif // SUPER_RM_H

