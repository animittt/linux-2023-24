#include <iostream>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>
#include <sys/stat.h>
#include<bits/stdc++.h>
#include <dirent.h>
#include <arg_parser/argument_parser.h>
#include <logging/logger.h>

const std::size_t buffer_size = 4096;
bool recursive = false;
const char* destination;
bool force = false;

void copyFile(const char* source, const char* dest)
{
    int file_from_id = open(source, O_RDONLY);
    struct stat fileStat{};
    stat(dest, &fileStat);
    int file_to_id;
    constexpr mode_t copy_to_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    if(S_ISDIR(fileStat.st_mode))
    {
        std::string source_str = std::string(source);
        std::string sourceName;
        if(source_str.find_last_of('/') != std::string::npos)
        {
            sourceName = source_str.substr(source_str.find_last_of('/'),source_str.length());
        }
        else
        {
            sourceName = "/" + source_str;
        }
        std::string fullPath = std::string(dest) + sourceName;
        file_to_id = open(fullPath.c_str(), O_CREAT | O_WRONLY, 0755);
    }
    else
    {
        file_to_id = open(dest, O_CREAT | O_WRONLY | O_TRUNC, copy_to_mode);
        if(file_to_id == -1)
        {
            std::cout << dest << "\n";
            LOG_FATAL("open ");
        }
    }
    if(file_from_id == -1)
    {
        LOG_FATAL("open ");
    }
    std::vector<char> buffer(buffer_size, '\0');
    ssize_t bytes_read;
    while(0 != (bytes_read = read(file_from_id, buffer.data(), buffer_size)))
    {
        if (bytes_read < 0)
        {
            LOG_FATAL("read failed ");
        }
        ssize_t bytes_written = write(file_to_id, buffer.data(), bytes_read);
        if (bytes_written < 0)
        {
            LOG_FATAL("write failed: ");
        }
    }
    close(file_to_id);
    close(file_from_id);
}

int ftw_callback(const char* source, const struct stat* sb, int typeflag)
{
    static bool madeDir = false;
    static int counter = 0;
    static std::string newDirectory;
    if(counter == 0)
    {
        ++counter;
        return 0;
    }
    if(typeflag == FTW_F)
    {
        std::string dest_str = std::string(source);
        std::string destName;
        if(dest_str.find_last_of('/') != std::string::npos)
        {
            destName = dest_str.substr(dest_str.find_last_of('/'),dest_str.length());
        }
        else
        {
            destName = "/" + dest_str;
        }
        if(madeDir)
        {
            if(newDirectory.find_last_of('/') != std::string::npos)
            {
                newDirectory = newDirectory.substr(newDirectory.find_last_of('/'),newDirectory.length());
            }
            else
            {
                newDirectory = "/" + dest_str;
            }
            newDirectory = destination + newDirectory + destName;
        }
        else
            newDirectory = destination + destName;
        copyFile(source,newDirectory.c_str());
    }
    else if(typeflag == FTW_D)
    {
        madeDir = true;
        recursive = true;
        std::string dest_str = std::string(source);
        std::string destName;s
        if(dest_str.find_last_of('/') != std::string::npos)
        {
            destName = dest_str.substr(dest_str.find_last_of('/'),dest_str.length());
        }
        else
        {
            destName = "/" + dest_str;
        }
        newDirectory = destination + destName;
        mkdir(newDirectory.c_str(),0755 );
    }
    return 0;
}

void copyDirectory(const char* source, const char* dest)
{
    if(!recursive)
    {
        LOG_WARNING(std::string(source) + " is a directory: Omitting.");
        return;
    }
    if(mkdir(dest, 0755 ) < 0)
    {
        if( errno == EEXIST)
        {
            struct stat destinationStat{};
            stat(dest, &destinationStat);
            if(!S_ISDIR(destinationStat.st_mode))
            {
                LOG_WARNING(std::string(dest) + " ");
            }
        }
        else
            LOG_ERROR(std::string(dest) + strerror(errno));
    }
    ftw(source, ftw_callback, 1);
}

bool removeDirectory(const char* path)
{
    DIR* dir = opendir(path);
    if (!dir)
    {
        return false;
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
                {
                    removeDirectory(fullPath.c_str());
                }
                else
                {
                    remove(fullPath.c_str());
                }
            }
        }
    }

    closedir(dir);

    if (rmdir(path) != 0)
    {
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    system("pwd");
    if(argc < 3)
    {
        LOG_FATAL("wrong arguments ");
    }
    std::vector<std::string> arguments;
    argument_parser parser(argc, argv, "rnf");
    for (auto it = parser.begin(); it != parser.end(); ++it)
    {
        if(it->_key == "r")
        {
            recursive = true;
        }
        else if(it->_key == "f")
        {
            force = true;
        }
        else
        {
            arguments.push_back(it->_key);
        }
    }
    if(arguments.size() < 2)
    {
        LOG_FATAL("wrong arguments");
    }
    destination = argv[argc - 1];
    arguments.pop_back();
    struct stat destStat{};
    int res = stat(destination, &destStat);

    if(res != 0)
    {
        if(errno == ENOENT)
        {
            if (S_ISREG(destStat.st_mode))
            {
                if(arguments.size() == 1)
                    open(destination, O_CREAT | O_WRONLY | O_TRUNC, 0755); //TODO check return value
                else
                    LOG_FATAL("target " + std::string(destination) + " is not a directory");
            }
            else if(S_ISDIR(destStat.st_mode))
            {
                if (force)
                {
                    removeDirectory(destination);
                    mkdir(destination, 0755);
                }
            }
            else if (arguments.size() > 1)
            {
                LOG_FATAL(std::string(destination) +" is not a directory");
            }
        }
        else
            LOG_FATAL(strerror(errno));
    }
    for(const auto & argument : arguments)
    {
        stat(argument.c_str(), &destStat);
        if(S_ISREG(destStat.st_mode))
        {
            copyFile(argument.c_str(), destination);
        }
        else if(S_ISDIR(destStat.st_mode))
        {
            copyDirectory(argument.c_str(), destination);
        }
    }
    return 0;
}
