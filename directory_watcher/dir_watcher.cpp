#include <sys/inotify.h>
#include <climits>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <logging/logger.h>
#include <sys/stat.h>
#include <ftw.h>

    static void /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
    printf(" wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);
    printf("mask = ");
    if (i->mask & IN_ACCESS) LOG_WARNING("IN_ACCESS ");
    if (i->mask & IN_ATTRIB) LOG_WARNING("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) LOG_WARNING("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE) LOG_WARNING("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE) LOG_WARNING("IN_CREATE ");
    if (i->mask & IN_DELETE) LOG_WARNING("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF) LOG_WARNING("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED) LOG_WARNING("IN_IGNORED ");
    if (i->mask & IN_ISDIR) LOG_WARNING("IN_ISDIR ");
    if (i->mask & IN_MODIFY) LOG_WARNING("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF) LOG_WARNING("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM) LOG_WARNING("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO) LOG_WARNING("IN_MOVED_TO ");
    if (i->mask & IN_OPEN) LOG_WARNING("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW) LOG_WARNING("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT) LOG_WARNING("IN_UNMOUNT ");
    printf("\n");
    if (i->len > 0)
        printf(" name = %s\n", i->name);
}
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
int main(int argc, char *argv[])
{
    int inotifyFd;
    int wd, j;
    char buf[BUF_LEN];
    ssize_t numRead;
    char *p;
    struct inotify_event *event;
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        LOG_INFO("%s pathname... \n" +  std::string(argv[0]));
    inotifyFd = inotify_init();
    if (inotifyFd == -1)
        LOG_FATAL("inotify_init" + std::string(strerror(errno)));

    //struct stat fileStat{};

    for (j = 1; j < argc; j++)
    {
        //stat(argv[j], &fileStat);
        wd = inotify_add_watch(inotifyFd, argv[j], IN_ALL_EVENTS);
        if (wd == -1)
            LOG_FATAL("inotify_add_watch" + std::string(strerror(errno)));
        printf("Watching %s using wd %d\n", argv[j], wd);
    }
    for (;;)
    {
        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0)
            LOG_FATAL("read() from inotify fd returned 0!");
        if (numRead == -1)
            LOG_FATAL("read" + std::string(strerror(errno)));
        printf("Read %ld bytes from inotify fd\n", (long) numRead);
        for (p = buf; p < buf + numRead; )
        {
            event = (struct inotify_event *) p;

            displayInotifyEvent(event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }
    exit(EXIT_SUCCESS);
}