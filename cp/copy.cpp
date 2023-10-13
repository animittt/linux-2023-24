#include <iostream>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>
#include <algorithm>
#include <vector>
#include <ftw.h>
#include <sys/stat.h>
#include<bits/stdc++.h>
#include <arg_parser/argument_parser.h>
#include <logging/logger.h>

const std::size_t buffer_size = 4096;
const char* destination;
bool recursive;
bool force;
bool noOverwrite;

void copyFile(const char* source, const char* dest)
{
    if(recursive)
    {
        LOG_ERROR(std::string(source) + "is not a directory");
        exit(EXIT_FAILURE);
    }
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
        file_to_id = open(fullPath.c_str(), O_CREAT | O_RDWR, copy_to_mode);
    }
    else
        file_to_id = open(dest, O_CREAT | O_RDWR  | O_TRUNC, copy_to_mode);
    if(file_from_id == -1)
    {
        LOG_ERROR("open ");
        exit(EXIT_FAILURE);
    }
    if(file_to_id == -1)
    {
        LOG_ERROR("open ");
        exit(EXIT_FAILURE);
    }
    std::vector<char> buffer(buffer_size, '\0');
    ssize_t bytes_read;
    while(0 != (bytes_read = read(file_from_id, buffer.data(), buffer_size)))
    {
        if (bytes_read < 0)
        {
            LOG_ERROR("read failed ");
            exit(EXIT_FAILURE);
        }
        ssize_t bytes_written = write(file_to_id, buffer.data(), bytes_read);
        if (bytes_written < 0)
        {
            LOG_ERROR("write failed: ");
            exit(EXIT_FAILURE);
        }
    }
    close(file_to_id);
    close(file_from_id);
}
bool madeDir = false;
int ftw_callback(const char* source, const struct stat* sb, int typeflag)
{
    static int counter = 0;
    static std::string newDirectory;
    if(counter == 0)
    {
        ++counter;
        return 0;
    }
    if(typeflag == FTW_F)
    {
        recursive = false;
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
        std::string destName;
        if(dest_str.find_last_of('/') != std::string::npos)
        {
            destName = dest_str.substr(dest_str.find_last_of('/'),dest_str.length());
        }
        else
        {
            destName = "/" + dest_str;
        }
        newDirectory = destination + destName;
        constexpr mode_t copy_to_mode = S_IRUSR | S_IXUSR | S_IRGRP | S_IROTH;
        mkdir(newDirectory.c_str(),0755 );
    }
    return 0;
}
void copyDirectory(const char* source, const char* dest)
{
    const char* originalDestination = destination;  // Save the original destination
    destination = dest;
    if(!recursive)
    {
        LOG_ERROR("-r not specified.");
        exit(EXIT_FAILURE);
    }
    ftw(source, ftw_callback, 1);
    destination = originalDestination;
}

int main(int argc, char** argv)
{
    recursive = false;
    force = false;
    noOverwrite = false;
    if(argc < 3)
    {
        LOG_ERROR("wrong arguments ");
        exit(EXIT_FAILURE);
    }
    std::vector<std::string> arguments;
    argument_parser parser(argc, argv, "rnf");
    for (auto it = parser.begin(); it != parser.end(); ++it)
    {
        if(it->_key == "r")
        {
            recursive = true;
        }
        else if(it->_key == "n")
        {
            noOverwrite = true;
            force = false;
        }
        else if(it->_key == "f")
        {
            force = true;
        }
        else
        {
            if(it->_key != argv[argc - 1])
            {
                arguments.push_back(it->_key);
            }
            else
                break;
        }
    }
    if(noOverwrite)
        force = false;
    destination = argv[argc - 1];
    struct stat fileStat{};
    int res = stat(destination, &fileStat);
    if(res != 0)
    {
        LOG_FATAL("vat es ara?");
    }
    if(arguments.size() > 1 && S_ISREG(fileStat.st_mode))
    {
        LOG_ERROR(std::string(destination) + " is not a directory");
        exit(EXIT_FAILURE);
    }
    for(const auto & argument : arguments)
    {
        stat(argument.c_str(), &fileStat);
        if(S_ISREG(fileStat.st_mode))
        {
            copyFile(argument.c_str(), destination);
        }
        else if(S_ISDIR(fileStat.st_mode))
        {
            copyDirectory(argument.c_str(), destination);
        }
    }
    return 0;
}