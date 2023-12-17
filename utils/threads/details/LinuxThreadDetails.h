#ifndef THREADAPI_LINUXTHREADDETAILS_H
#define THREADAPI_LINUXTHREADDETAILS_H

#include <thread>
#include <cstring>
#include <typeinfo>
#include <functional>

namespace detail
{

    template <typename Lambda>
    void* posix_thread_routine(void* binded_function)
    {
        auto* wrapped_function = reinterpret_cast<Lambda*>(binded_function);
        (*wrapped_function)();

        delete wrapped_function;
        return nullptr;
    }

    template <typename Callable, typename... Args>
    class ThreadRoutineWrapper
    {
        std::decay_t<Callable> m_callable;
        std::tuple<std::decay_t<Args>...> m_argumentsTuple;

    public:
        explicit ThreadRoutineWrapper(Callable&& c, Args&&... args)
                : m_callable{std::forward<std::decay_t<Callable&&>>(c)}
                , m_argumentsTuple{std::forward<std::decay_t<Args&&>>(args)...}
        {

        }

        void operator()()
        {
            std::apply(m_callable, m_argumentsTuple);
        }

    };

} // namespace detail

#endif //THREADAPI_LINUXTHREADDETAILS_H
