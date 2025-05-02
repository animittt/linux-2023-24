// commissioner.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

void handle_response(int signal, siginfo_t *info, void *context)
{
    if (signal == SIGILL)
    {
        std::cout << "Received response from the spy process.\n";
        pid_t senderPID = info->si_pid;
        std::cout << "Spionen of bourjois, western, anticommunist, visual studio/windows using scum detected with PID: " << senderPID << std::endl;
        std::cout << "Sending SIGKILL to terminate the spy process.\n";
        kill(senderPID, SIGKILL);
    }
}

void send_signal_to_spy(int pid)
{
    std::cout << "Sending SIGCONT to a process with PID: " << pid << std::endl;
    kill(pid, SIGCONT);
    usleep(10000);
}

int main()
{
    std::ifstream file("/home/mitani/linux-2024-25/hw17_back_to_the_ussr/postman.txt");
    std::vector<pid_t> pids;

    if (!file.is_open())
    {
        std::cerr << "Unable to open file 'postman.txt'. Exiting.\n";
        return 1;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        pid_t pid;
        while (iss >> pid)
            pids.push_back(pid);
    }
    file.close();

    struct sigaction sa{};
    sa.sa_sigaction = handle_response;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGILL, &sa, nullptr);

    std::cout << "Checking for spy processes...\n";
    for (pid_t pid : pids)
    {
        std::string procPath = "/proc/" + std::to_string(pid);
        if (kill(pid,0) == 0)
        {
            std::cout << "Checking process with PID: " << pid << std::endl;
            send_signal_to_spy(pid);
        }
    }

    std::cout << "All spy processes handled.\n";
    return 0;
}
