#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <random>
#include <chrono>
#include "blocking_queue.h"

constexpr size_t requestsLimit = 16;
BlockingQueue<double> requestsQueue(requestsLimit);
std::mutex requestsMutex;
std::condition_variable requestsState;
bool engineStop = false;

void producer()
{
    std::random_device rd;
    std::mt19937_64 mt{ rd() };
    auto nd = std::normal_distribution(0., 1.);

    while (true)
    {
        auto tmp = nd(mt);
        {
            std::unique_lock lock(requestsMutex);
            requestsState.wait(lock, [&]() { return requestsQueue.size() != requestsLimit; });
            if (engineStop)
                break;
            requestsQueue.Enqueue(tmp);
            std::cout << "producer: " << tmp << "\n";
        }
        requestsState.notify_all();
    }
}

void consumer()
{
    while (true)
    {
        double retrievedValue = 0.;
        {
            std::unique_lock lock(requestsMutex);
            requestsState.wait(lock, [&]() { return !requestsQueue.empty() || engineStop; });

            if (engineStop && requestsQueue.empty())
                break;

            if (!requestsQueue.empty())
                retrievedValue = requestsQueue.Dequeue();
        }
        requestsState.notify_all();
        std::cout << "consumer: "<< retrievedValue << "\n";
    }
}


void demo3_producer_consumer()
{
    using namespace std::chrono_literals;
    std::thread t1{ producer };
    std::thread t2{ consumer };

    std::this_thread::sleep_for(5s);
    engineStop = true;
    t1.join();
    t2.join();
}

int main()
{
    demo3_producer_consumer();
    return 0;
}
