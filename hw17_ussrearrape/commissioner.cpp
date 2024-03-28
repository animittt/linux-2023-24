// commissioner.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

bool responded = false;

void handle_spy(pid_t pid)
{
    std::cout << "Spionen of bourjois, western, anticommunist, visual studio/windows using scum detected with PID: " << pid << std::endl;
    std::cout << "Sending SIGKILL to terminate the spy process.\n";
    kill(pid, SIGKILL);
}

void handle_response(int signal)
{
    if (signal == SIGILL)
    {
        std::cout << "Received response from the spy process.\n";
        responded = true;
    }
}

void send_signal_to_spy(int pid)
{
    std::cout << "Sending SIGUSR1 to a process with PID: " << pid << std::endl;
    kill(pid, SIGCONT);
}

int main()
{
    std::ifstream file("postman.txt");
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

    std::cout << "Checking for spy processes...\n";
    struct sigaction sa{};
    sa.sa_handler = handle_response;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGILL, &sa, nullptr);

    for (pid_t pid : pids)
    {
        if (kill(pid, 0) == 0)
        {
            std::cout << "Checking process with PID: " << pid << std::endl;
            send_signal_to_spy(pid);
            usleep(10000);
            if(responded)
                handle_spy(pid);
            responded = false;
        }
    }

    std::cout << "All spy processes handled.\n";
    return 0;
}
