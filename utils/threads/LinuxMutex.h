#ifndef THREADAPI_LINUXMUTEX_H
#define THREADAPI_LINUXMUTEX_H
#include <stdexcept>
#include <cstring>
#include <logger.h>
namespace rau
{

    class LinuxMutex
    {
    public:
        using native_handle_type = pthread_mutex_t;

    private:
        pthread_mutex_t mutex{};

    public:
        constexpr LinuxMutex() noexcept
        {
            if (pthread_mutex_init(&mutex, nullptr) != 0)
            {
                LOG_ERROR("Failed to initialize mutex " + std::string(strerror(errno)));
            }
        }

        ~LinuxMutex()
        {
            if (pthread_mutex_destroy(&mutex) != 0)
            {
                LOG_ERROR("Failed to destroy the mutex " + std::string(strerror(errno)));
            }
        }

        [[nodiscard]] native_handle_type native_handle() const noexcept
        {
            return mutex;
        }

        void lock()
        {
            if (pthread_mutex_lock(&mutex) != 0)
            {
                throw std::system_error(std::make_error_code(std::errc::resource_deadlock_would_occur));
            }
        }

        void unlock() noexcept
        {
            if (pthread_mutex_unlock(&mutex) != 0)
            {
                LOG_ERROR("Failed to unlock the mutex " + std::string(strerror(errno)));
            }
        }

        bool try_lock() noexcept
        {
            return pthread_mutex_trylock(&mutex) == 0;
        }
    };
}

namespace rau
{
    class LinuxMutexGuard
    {
    private:
        LinuxMutex& mutex;

    public:
        explicit LinuxMutexGuard(LinuxMutex& m) : mutex(m)
        {
            mutex.lock();
        }

        ~LinuxMutexGuard()
        {
            mutex.unlock();
        }

        LinuxMutexGuard(const LinuxMutexGuard&) = delete;
        LinuxMutexGuard& operator=(const LinuxMutexGuard&) = delete;
        LinuxMutexGuard(LinuxMutexGuard&&) = delete;
        LinuxMutexGuard& operator=(LinuxMutexGuard&&) = delete;
    };
} // namespace rau

#endif //THREADAPI_LINUXMUTEX_H