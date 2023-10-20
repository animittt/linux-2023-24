#ifndef DO_MAGIC
#define DO_MAGIC

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <logging/logger.h>

void do_magic()
{
    int fd1 = open("exclusive_file.log", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    int saved_stdout = dup(1);
    if (dup2(fd1, 1) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    std::cout << "First line." << std::endl;
    std::cout << "Second line." << std::endl;

    close(fd1);
    if(dup2(saved_stdout, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    int fd = open("new_pts", O_RDONLY, S_IRUSR);
    if (dup2(fd, 0) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
}
#endif
