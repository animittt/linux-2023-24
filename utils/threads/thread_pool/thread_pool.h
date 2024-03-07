#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <optional>
#include "blocking_queue.h"

class ThreadPool
{
public:
    explicit ThreadPool(std::size_t num_threads = std::thread::hardware_concurrency()) : stop(false)
    {
        for (std::size_t i = 0; i < num_threads; ++i)
        {
            workers.emplace_back([this]
            {
                while (true)
                {
                    std::optional<std::function<void()>> maybe_task = tasks.Dequeue();
                    if (!maybe_task.has_value() || !*maybe_task)
                        break;
                    std::function<void()> task = std::move(maybe_task.value());
                    task();
                }
            });
        }
    }

    template<class F, class... Args>
    auto push(F&& f, Args&&... args) -> std::future<decltype(f(args...))> 
    {
        using return_type = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        std::future<return_type> res = task->get_future();
        tasks.Enqueue([task](){ (*task)(); });
        return res;
    }

    ~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop = true;
        }
        for (std::thread& worker : workers)
            tasks.Stop();
        for (std::thread& worker : workers)
            worker.join();
    }

private:
    BlockingQueue<std::function<void()>> tasks;
    std::vector<std::thread> workers;
    std::mutex mutex_;
    bool stop;
};
