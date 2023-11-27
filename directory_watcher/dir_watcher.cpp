#include <sys/inotify.h>
#include <climits>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <logging/logger.h>

  static void displayInotifyEvent(struct inotify_event *i)
{
    LOG_INFO("wd = " + std::to_string(i->wd));
    if (i->cookie > 0)
        LOG_INFO ("cookie = " + std::to_string(i->cookie) + '\n');
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
    if (i->len > 0)
        LOG_INFO("name = " + std::string(i->name) +'\n');
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
    {
        LOG_INFO("pathname... \n" +  std::string(argv[0]));
        return 0;
    }
    inotifyFd = inotify_init();
    if (inotifyFd == -1)
        LOG_FATAL("inotify_init " + std::string(strerror(errno)));
    for (j = 1; j < argc; ++j)
    {
        wd = inotify_add_watch(inotifyFd, argv[j], IN_ALL_EVENTS);
        if (wd == -1)
            LOG_FATAL("inotify_add_watch " + std::string(strerror(errno)));
        std::cout << "Watching" <<  argv[j] << " using " << wd << '\n';
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
            event = (struct inotify_event*) p;

            displayInotifyEvent(event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }
    exit(EXIT_SUCCESS);
}