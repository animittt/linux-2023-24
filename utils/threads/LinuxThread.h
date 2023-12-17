#ifndef THREADAPI_LINUXTHREAD_H
#define THREADAPI_LINUXTHREAD_H

#include <cstring>
#include <typeinfo>
#include <functional>

namespace rau
{

    class LinuxThread
    {
    public:
        using native_handle_type = pthread_t;

    public:

        // The class LinuxThread::id is a lightweight, trivially copyable class that serves as
        // a unique identifier of LinuxThread.
        //
        // Instances of this class may also hold the special distinct value that
        // does not represent any thread. Once a thread has finished,
        // the value of LinuxThread::id may be reused by another thread.
        //
        // This class is designed for use as key in associative containers, both ordered and unordered.
        class id
        {
            native_handle_type m_threadID{0ull};
        public:

            friend bool operator==(id lhs, id rhs) noexcept
            {
                return 0 != pthread_equal(lhs.m_threadID, rhs.m_threadID);
            }


            friend bool operator!=( id lhs, id rhs ) noexcept
            {
                return !(lhs == rhs);
            }

            friend bool operator<( id lhs, id rhs ) noexcept
            {
                return lhs.m_threadID < rhs.m_threadID;
            }

            friend bool operator<=(id lhs, id rhs) noexcept
            {
                return !(rhs < lhs);
            }

            friend bool operator>(id lhs, id rhs) noexcept
            {
                return rhs < lhs;
            }

            friend bool operator>=(id lhs, id rhs) noexcept
            {
                return !(lhs < rhs);
            }

            friend std::ostream& operator<<(std::ostream& out, id obj)
            {
                out << obj.m_threadID;
                return out;
            }

            friend std::hash<LinuxThread::id>;
        };

    private:
        id m_ID;

    public:

        native_handle_type native_handle()
        {
            return *reinterpret_cast<native_handle_type*>(&m_ID);
        }

        LinuxThread() noexcept : m_ID() {}

        template <typename Callable, typename ...Args>
        explicit LinuxThread(Callable&& c, Args&&... args) noexcept
        {
            auto* wrapper = new detail::ThreadRoutineWrapper<Callable&&, Args&&...>
            {std::forward<Callable&&>(c), std::forward<Args&&>(args)...};
            using lambda_type = std::remove_reference_t<decltype(*wrapper)>;
            native_handle_type temp_id;
            auto result = pthread_create(&temp_id, nullptr,
                                         &detail::posix_thread_routine<lambda_type>, wrapper);
            if(result != 0)
            {
                delete wrapper;
                throw std::runtime_error{strerror(errno)};
            }
            m_ID = *reinterpret_cast<id*>(temp_id);
        }

        LinuxThread(LinuxThread&& other) noexcept
        {
            m_ID = other.m_ID;
            other.m_ID = {};
        }

        LinuxThread( const LinuxThread& ) = delete;

        ~LinuxThread()
        {
            if (joinable())
                std::terminate();
        }

        LinuxThread& operator=(LinuxThread&& other) noexcept
        {
            if (joinable())
                std::terminate();
            m_ID = other.m_ID;
            other.m_ID = {};
            return *this;
        }

        LinuxThread& operator=(LinuxThread& other) = delete;

        [[nodiscard]]
        id get_id() const
        {
            return m_ID;
        }


        [[nodiscard]]
        bool joinable() const
        {
            return get_id() != id();
        }
        void join()
        {
            if (joinable())
            {
                int result = pthread_join(native_handle(), nullptr);
                if (result != 0)
                {
                    throw std::runtime_error("Failed to join the thread " + std::string(strerror(errno)));
                }
                m_ID = {};
            }
        }

        void detach()
        {
            if (joinable())
            {
                int result = pthread_detach(native_handle());
                if (result != 0)
                {
                    throw std::runtime_error("Failed to detach the thread " + std::string(strerror(errno)));
                }
                m_ID = {};
            }
        }
    };

}

namespace std
{
    template <>
    struct hash<rau::LinuxThread::id>
    {
        size_t operator()(rau::LinuxThread::id&& id)
        {
            return id.m_threadID;
        }
    };

} // namespace std

#endif //THREADAPI_LINUXTHREAD_H
