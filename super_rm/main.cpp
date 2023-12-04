#include <iostream>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <argument_parser.h>
#include <logger.h>
#include "super_rm.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        LOG_ERROR("missing operand");
        return 1;
    }
    bool recursive = false;
    bool verbose = false;
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
    int res;
    for(const auto& filename : arguments)
    {
        res = stat(filename.c_str(), &fileStat);
        if(res != 0)
        {
            LOG_WARNING("cannot remove " + filename + ": " + strerror(errno));
            continue;
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
