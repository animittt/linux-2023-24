#ifndef THREADAPI_LINUXMUTEX_H
#define THREADAPI_LINUXMUTEX_H
#include <stdexcept>
#include <cstring>

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
                throw std::runtime_error("Failed to initialize mutex " + std::string(strerror(errno)));
            }
        }

        ~LinuxMutex()
        {
            if (pthread_mutex_destroy(&mutex) != 0)
            {
                throw std::runtime_error("Failed to destroy the mutex " + std::string(strerror(errno)));
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
                throw std::runtime_error("Failed to lock the mutex " + std::string(strerror(errno)));
            }
        }

        void unlock()
        {
            if (pthread_mutex_unlock(&mutex) != 0)
            {
                throw std::runtime_error("Failed to unlock the mutex " + std::string(strerror(errno)));
            }
        }

        bool try_lock()
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