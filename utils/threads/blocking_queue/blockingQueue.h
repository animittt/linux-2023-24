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
    explicit BlockingQueue(std::size_t capacity = 20)
            : m_capacity(capacity)
            , m_stop(false)
    {}

    void Enqueue(const T& item)
    {
        {
            std::unique_lock lock(m_mutex);
            m_condition.wait(lock, [this] { return m_queue.size() < m_capacity || m_stop.test(); });
            m_queue.push(item);
        }
        m_condition.notify_one();
    }

    std::size_t size()
    {
        std::unique_lock lock(m_mutex);
        return m_queue.size();
    }

    bool empty()
    {
        std::unique_lock lock(m_mutex);
        return m_queue.empty();
    }

    T Dequeue()
    {
        std::unique_lock lock(m_mutex);
        m_condition.wait(lock, [this] { return !m_queue.empty() || m_stop.test(); });
        T item = std::move(m_queue.front());
        m_queue.pop();
        return item;
    }

    ~BlockingQueue()
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_stop.test_and_set();
        }
        m_condition.notify_all();
    }

private:
    std::queue<T> m_queue;
    std::size_t m_capacity;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic_flag m_stop;
};