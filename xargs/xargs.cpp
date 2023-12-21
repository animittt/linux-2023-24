#include <iostream>
#include <vector>
#include <logger.h>
#include <unistd.h>
#include <cstring>

void executeCommand(const std::string& command, const std::vector<std::string>& arguments)
{
    std::vector<const char*> args;
    args.push_back(command.c_str());
    for (const auto& arg : arguments)
    {
        args.push_back(arg.c_str());
    }
    args.push_back(nullptr);
    if (execvp(command.c_str(), const_cast<char* const*>(args.data())) == -1)
    {
        LOG_FATAL("Error executing the command " + std::string(strerror(errno)));
    }
}

int main(int argc, char *argv[])
{
    std::string command;
    std::vector<std::string> arguments;
    if (argc < 2)
    {
        LOG_FATAL("too few arguments");
    }
    command = argv[1];
    std::string arg;
    while (std::cin >> arg)
    {
        arguments.push_back(arg);
    }
    executeCommand(command, arguments);
    return 0;
}