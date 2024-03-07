#pragma once

#include <iostream>
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <random>
#include <chrono>

template <typename T>
class BlockingQueue
{
public:
    BlockingQueue()
            : stop_(false) {}

    void Enqueue(const T& item)
    {
        {
            std::unique_lock lock(mutex_);
            queue_.push(item);
        }
        condition_.notify_one();
    }

    std::size_t size()
    {
        std::unique_lock lock(mutex_);
        return queue_.size();
    }

    bool empty()
    {
        std::unique_lock lock(mutex_);
        return queue_.empty();
    }

    T Dequeue()
    {
        std::unique_lock lock(mutex_);
        condition_.wait(lock, [this] { return !queue_.empty() || stop_; });

        if (queue_.empty() && stop_)
            return T{};

        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    void Stop()
    {
        {
            std::unique_lock lock(mutex_);
            stop_ = true;
        }
        condition_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool stop_;
};
