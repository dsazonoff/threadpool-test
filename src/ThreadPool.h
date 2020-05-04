#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "Task.h"


namespace core
{

class ThreadPool
{
public:
    explicit ThreadPool(unsigned int nThreads = 0);
    ~ThreadPool();
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <typename T>
    void push(std::unique_ptr<T>&& task);
    template <typename It>
    void push(const It& begin, const It& end);
    void stop(bool clearQueue = false);
    size_t threadCount() const;

private:
    void notifyThreads();

private:
    static void ThreadFn(ThreadPool& pool);

private:
    std::vector<std::thread> _threads;
    std::queue<std::unique_ptr<Task>> _queue;
    std::mutex _m;
    std::condition_variable _cv;
    std::atomic_bool _isRunning = true;
};

template <typename T>
void ThreadPool::push(std::unique_ptr<T>&& task)
{
    static_assert(std::is_base_of_v<Task, T>);

    const std::unique_lock lock{_m};
    _queue.emplace(std::move(task));
    notifyThreads();
}

template <typename It>
void ThreadPool::push(const It& begin, const It& end)
{
    const std::unique_lock lock{_m};
    std::for_each(begin, end, [&q = _queue](auto& task)
    {
        q.emplace(std::move(task));
    });
    notifyThreads();
}

}