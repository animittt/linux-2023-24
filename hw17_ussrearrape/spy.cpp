#include <iostream>
#include <csignal>
#include <unistd.h>

void handle_sigcont(int signal, siginfo_t *info, void *context)
{
    if (signal == SIGCONT)
    {
        pid_t senderPID = info->si_pid;
        std::cout << "SIGHEIL, mein Führer!\n";
        kill(senderPID, SIGILL);
    }
}

int main()
{
    struct sigaction sa{};
    sa.sa_sigaction = handle_sigcont;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGCONT, &sa, nullptr) == -1)
    {
        std::cerr << "Error setting up signal handler for SIGCONT.\n";
        return 1;
    }
    std::cout << "Spy process started with PID: " << getpid() << '\n';
    std::cout << "Waiting for instructions, mein Führer!\n";
    while (true) //because it's an evil spy, why not ?
        pause();
    return 0;
}
