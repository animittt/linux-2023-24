#include <iostream>
#include <string>
#include <stack>
#include <utility>
#include <dirent.h>
#include <sys/stat.h>

class RecursiveDirectoryIterator
{
public:
    explicit RecursiveDirectoryIterator(const std::string& path)
    {
        dirStack.emplace(path, nullptr);
        advance();
    }

    RecursiveDirectoryIterator& operator++()
    {
        advance();
        return *this;
    }

    const std::string& operator*() const
    {
        return currentEntry;
    }

    bool operator!=(const RecursiveDirectoryIterator& other) const
    {
        return dirStack != other.dirStack || currentEntry != other.currentEntry;
    }

private:
    std::stack<std::pair<std::string, DIR*>> dirStack;
    std::string currentEntry;
    void advance()
    {
        while (!dirStack.empty())
        {
            auto& top = dirStack.top();
            if (top.second == nullptr)
            {
                top.second = opendir(top.first.c_str());
                if (top.second == nullptr)
                {
                    dirStack.pop();
                    continue;
                }
            }
            struct dirent* entry = readdir(top.second);
            while (entry != nullptr) {
                if (entry->d_type == DT_DIR && std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
                {
                    dirStack.emplace(top.first + "/" + entry->d_name, nullptr);
                    currentEntry = top.first + "/" + entry->d_name;
                    return;
                }
                else if (entry->d_type == DT_REG)
                {
                    currentEntry = top.first + "/" + entry->d_name;
                    return;
                }
                entry = readdir(top.second);
            }
            closedir(top.second);
            dirStack.pop();
        }
        currentEntry.clear();
    }
};

class Directory
{
public:
    explicit Directory(std::string  path)
    : path(std::move(path)) {}

    [[nodiscard]] RecursiveDirectoryIterator begin() const
    {
        return RecursiveDirectoryIterator(path);
    }

    [[nodiscard]] RecursiveDirectoryIterator end() const
    {
        return RecursiveDirectoryIterator("");
    }

private:
    std::string path;
};

