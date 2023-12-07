#include <sys/inotify.h>
#include <climits>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <logger.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
  const char* filename = "/home/mitani/linux-2024-25/directory_watcher/logfile.log";
  static void displayInotifyEvent(struct inotify_event *i)
{
    LOG_INFO(filename,"wd = " + std::to_string(i->wd));
    if (i->cookie > 0)
        LOG_INFO (filename,"cookie = " + std::to_string(i->cookie) + '\n');
    if (i->mask & IN_ACCESS) LOG_WARNING(filename,"IN_ACCESS ");
    if (i->mask & IN_ATTRIB) LOG_WARNING(filename,"IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) LOG_WARNING(filename,"IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE) LOG_WARNING(filename,"IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE) LOG_WARNING(filename,"IN_CREATE ");
    if (i->mask & IN_DELETE) LOG_WARNING(filename,"IN_DELETE ");
    if (i->mask & IN_DELETE_SELF) LOG_WARNING(filename,"IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED) LOG_WARNING(filename,"IN_IGNORED ");
    if (i->mask & IN_ISDIR) LOG_WARNING(filename,"IN_ISDIR ");
    if (i->mask & IN_MODIFY) LOG_WARNING(filename,"IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF) LOG_WARNING(filename,"IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM) LOG_WARNING(filename,"IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO) LOG_WARNING(filename,"IN_MOVED_TO ");
    if (i->mask & IN_OPEN) LOG_WARNING(filename,"IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW) LOG_WARNING(filename,"IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT) LOG_WARNING(filename,"IN_UNMOUNT ");
    if (i->len > 0)
        LOG_INFO(filename,"name = " + std::string(i->name) +'\n');
}
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
int main(int argc, char *argv[])
{
    pid_t daemon = fork();
    if(daemon != 0)
        exit(0);
    if(setsid() == -1)
    {
       LOG_ERROR(filename,"setsid");
        exit(1);
    }
    if(chdir("/"))
    {
        LOG_ERROR(filename,"chdir");
        exit(1);
    }
    for(int i = 0; i < 1023; ++i)
        close(i);
    open("/dev/null", O_RDWR);
    dup(0);
    dup(0);
    //sleep(10);
    int inotifyFd;
    int wd, j;
    char buf[BUF_LEN];
    ssize_t numRead;
    char *p;
    struct inotify_event *event;
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
    {
        LOG_INFO(filename,"pathname... \n" +  std::string(argv[0]));
        return 0;
    }
    inotifyFd = inotify_init();
    if (inotifyFd == -1)
        LOG_FATAL(filename,"inotify_init " + std::string(strerror(errno)));
    for (j = 1; j < argc; ++j)
    {
        wd = inotify_add_watch(inotifyFd, argv[j], IN_ALL_EVENTS);
        if (wd == -1)
            LOG_FATAL(filename,"inotify_add_watch " + std::string(strerror(errno)));
    }
    for (;;)
    {
        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0)
            LOG_FATAL(filename,"read() from inotify fd returned 0!");
        if (numRead == -1)
            LOG_FATAL(filename,"read" + std::string(strerror(errno)));
        for (p = buf; p < buf + numRead; )
        {
            event = (struct inotify_event*) p;

            displayInotifyEvent(event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }
    exit(EXIT_SUCCESS);
}
