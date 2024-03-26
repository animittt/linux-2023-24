
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <optional>
#include "blockingQueue.h"

class ThreadPool
{
public:
    explicit ThreadPool(std::mutex& mutex, std::condition_variable& condition, std::size_t num_threads = std::thread::hardware_concurrency())
            : m_stop(false)
            , m_condition(condition)
            , m_mutex(mutex)
    {
        for (std::size_t i = 0; i < num_threads; ++i)
        {
            m_workers.emplace_back([this]
                                   {
                                       while (true)
                                       {
                                           std::function<void()> task;
                                           {
                                               std::unique_lock<std::mutex> lock(m_mutex);
                                               m_condition.wait(lock, [this] { return !m_tasks.empty() || m_stop; });
                                               if (m_stop && m_tasks.empty())
                                                   return;
                                               task = std::move(m_tasks.Dequeue());
                                           }
                                           task();
                                       }
                                   });
        }
    }

    template<class F, class... Args>
    auto push(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
    {
        using return_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<return_type()>>([f = std::forward<F>(f), tup = std::make_tuple(std::forward<Args>(args)...)]() mutable {
            return std::apply(f, tup);
        });
        std::future<return_type> res = task->get_future();
        m_tasks.Enqueue([task](){ (*task)(); });
        return res;
    }

    ~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stop = true;
        }
        m_condition.notify_all();
        for (std::thread& worker : m_workers)
            worker.join();
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tasks.empty();
    }

private:
    std::vector<std::thread> m_workers;
    std::mutex& m_mutex;
    std::condition_variable& m_condition;
    BlockingQueue<std::function<void()>> m_tasks;
    bool m_stop;
};
