#include <iostream>
#include <arg_parser/argument_parser.h>
#include <logging/logger.h>
#include <fstream>
#include <string>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include<bits/stdc++.h>
#include <dirent.h>
#include <cstdlib>
#include <ctime>

void secureDeleteFile(const std::string &filename, bool verbose)
{
    std::fstream file(filename, std::ios::out | std::ios::binary);
    if (file.is_open())
    {
        srand(static_cast<unsigned int>(time(0)));
        for (size_t i = 0; i < 10; ++i)
        {
            char randomData[1024];
            for (int j = 0; j < 1024; ++j)
                randomData[j] = rand() % 256;
            file.write(randomData, sizeof(randomData));
        }
        file.close();
        remove(filename.c_str());
        if (verbose)
            std::cout << "Securely deleted file: " << filename << std::endl;
    }
    else
        LOG_ERROR("Error opening file: " + std::string(filename));

}
bool removeDirectory(const char* path, bool verbose)
{
    DIR* dir = opendir(path);
    if (!dir)
    {
        LOG_ERROR("cannot remove " + std::string(path) + ": " + strerror(errno));
    };
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
        std::cout << "Securely deleted directory: " << path << std::endl;
    return true;
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        LOG_ERROR("missing operand");
        return 1;
    }
    bool recursive;
    bool verbose;
    std::vector<std::string> arguments;
    argument_parser parser(argc, argv, "rv");
    for (auto it = parser.begin(); it != parser.end(); ++it)
    {
        if(it->_key == "r")
        {
            recursive = true;
        }
        else if(it->_key == "v")
        {
            verbose = true;
        }
        else
        {
            arguments.push_back(it->_key);
        }
    }
    struct stat fileStat{};
    int res{};
    for(const auto& filename : arguments)
    {
        res = stat(filename.c_str(), &fileStat);
        if(res != 0)
        {
            LOG_ERROR("cannot remove " + filename + ": " + strerror(errno));
            return 1;
        }
        if(S_ISREG(fileStat.st_mode))
        {
            secureDeleteFile(filename, verbose);
        }
        else if(S_ISDIR(fileStat.st_mode))
        {
            if(!recursive)
            {
                 LOG_ERROR("-r is not set");
                 return 1;
            }
            removeDirectory(filename.c_str(), verbose);
        }
    }
    return 0;
}

