#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <logger.h>
#include <cstring>
#include <string>

int do_command(const char* argv[])
{
    pid_t child_pid = fork();
    if (child_pid == -1)
    {
        LOG_FATAL("Fork failed" + std::string(strerror(errno)));
    }
    else if (child_pid == 0)
    {
        int count = 0;
        while (argv[count] != nullptr)
        {
            ++count;
        }

        char** exec_args = new char*[count + 1];
        for (int i = 0; i < count; ++i)
        {
            exec_args[i] = strdup(argv[i]);
        }
        exec_args[count] = nullptr;

        execvp(exec_args[0], exec_args);
        for (int i = 0; i < count; ++i)
        {
            free(exec_args[i]);
        }
        delete[] exec_args;
        LOG_FATAL("execvp error" + std::string(strerror(errno)));
    }
    else
    {
        int status;
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }
        else
        {
            LOG_FATAL("Child process didn't terminate normally" + std::string(strerror(errno)));
        }
    }
    return -1;
}

int main()
{
    const char* program[4];
    program[0] = "ls";
    program[1] = "-a";
    program[2] = "-l";
    program[3] = nullptr;
    int exit_code1 = do_command(program);
    program[0] = "cat";
    program[1] = "/etc/lsb-release";
    program[2] = nullptr;
    int exit_code2 = do_command(program);
    std::cout << "Exit code 1: " << exit_code1 << '\n';
    std::cout << "Exit code 2: " << exit_code2 << '\n';
    return 0;
}