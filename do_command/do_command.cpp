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
        LOG_FATAL(nullptr, "Fork failed" + std::string(strerror(errno)));
        return -1;
    }
    else if (child_pid == 0)
    {
        execvp(argv[0], const_cast<char* const*>(argv));
        LOG_ERROR(nullptr, "execvp error" + std::string(strerror(errno)));
        exit(EXIT_FAILURE);
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
            LOG_FATAL(nullptr, "Child process didn't terminate normally" + std::string(strerror(errno)));
            return -1;
        }
    }
}

int main()
{
    const char* program1[4];
    program1[0] = "ls";
    program1[1] = "-a";
    program1[2] = "-l";
    program1[3] = nullptr;

    const char* program2[3];
    program2[0] = "cat";
    program2[1] = "/etc/lsb-release";
    program2[2] = nullptr;

    int exit_code1 = do_command(program1);
    int exit_code2 = do_command(program2);

    std::cout << "Exit code 1: " << exit_code1 << '\n';
    std::cout << "Exit code 2: " << exit_code2 << '\n';

    return 0;
}
