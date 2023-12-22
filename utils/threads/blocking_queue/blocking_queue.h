//
// Created by mitani on 12/22/23.
//
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
    explicit BlockingQueue(std::size_t capacity)
            : capacity_(capacity), stop_(false) {}

    void Enqueue(const T& item)
    {
        {
            std::unique_lock lock(mutex_);
            condition_.wait(lock, [this] { return queue_.size() < capacity_ || stop_; });

            if (stop_)
            {
                return;
            }

            queue_.push(item);
        }
        condition_.notify_one();
    }

    std::size_t size()
    {
        return queue_.size();
    }

    bool empty()
    {
        return queue_.empty();
    }

    T Dequeue()
    {
        std::unique_lock lock(mutex_);
        condition_.wait(lock, [this] { return !queue_.empty() || stop_; });

        if (queue_.empty() && stop_) {
            return false;
        }

        T& item = queue_.front();
        queue_.pop();

        condition_.notify_one();
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
    std::size_t capacity_;
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool stop_;
};

