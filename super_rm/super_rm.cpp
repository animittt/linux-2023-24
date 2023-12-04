#include <random>
#include <bits/stdc++.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <argument_parser.h>
#include <logger.h>
#include "super_rm.h"

std::vector<char> generateRandomVector()
{
    static thread_local std::random_device rd;
    static thread_local std::mt19937 mt{rd()};
    static thread_local std::uniform_int_distribution<int> dist(0,255);
    std::vector<char> vec(blockSize);
    std::generate(vec.begin(), vec.end(), [](){return dist(mt);});
    return vec;
}

std::size_t getFileSize(int fd)
{
    struct stat fileStats{};
    int res = fstat(fd, &fileStats);
    if(res != 0)
    {
        LOG_ERROR(std::string(strerror(errno)));
        throw std::runtime_error("can't get file size.");
    }
    return fileStats.st_size;
}

void secureDeleteFile(const std::string &filename, bool verbose)
{
    int res = open(filename.c_str(), O_WRONLY);
    if(res == -1)
        LOG_FATAL("can't open file");
    std::size_t fileSize = getFileSize(res);
    std::vector<char> randVec = generateRandomVector();
    std::size_t bytesWritten = 0;
    while(bytesWritten < fileSize)
    {
        ssize_t writeRes =  write(res, randVec.data(), blockSize);
        if(writeRes == -1)
        {
            LOG_ERROR(strerror(errno));
        }
        else
            bytesWritten += writeRes;
    }
    remove(filename.c_str());
    if(verbose)
        LOG_INFO("Securely deleted file: " + filename );
}

bool removeDirectory(const char* path, bool verbose)
{
    DIR* dir = opendir(path);
    if (!dir)
    {
        LOG_ERROR("cannot remove " + std::string(path) + ": " + strerror(errno));
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            std::string fullPath = std::string(path) + "/" + entry->d_name;
            struct stat statBuf{};
            if (stat(fullPath.c_str(), &statBuf) == 0)
            {
                if (S_ISDIR(statBuf.st_mode))
                    removeDirectory(fullPath.c_str(), verbose);
                else
                    secureDeleteFile(fullPath, verbose);
            }
        }
    }
    closedir(dir);

    if (rmdir(path) != 0)
        return false;

    if (verbose)
        LOG_INFO("Securely deleted directory: " + std::string(path));
    return true;
}

