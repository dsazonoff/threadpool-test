#include "ThreadPool.h"

#include <cassert>


namespace core
{


ThreadPool::ThreadPool(unsigned int nThreads)
{
    if (nThreads == 0)
        nThreads = std::thread::hardware_concurrency();
    assert(nThreads > 0);

    _threads.reserve(nThreads);
    for (auto i = 0; i < nThreads; ++i)
        _threads.emplace_back(&ThreadFn, std::ref(*this));
}

ThreadPool::~ThreadPool()
{
    if (_isRunning)
        stop();
}

void ThreadPool::stop(bool clearQueue)
{
    std::unique_lock lock{_m};
    if (clearQueue)
        _queue = {};

    _isRunning = false;
    _cv.notify_all();
    lock.unlock();
    for (auto& t : _threads)
        t.join();
}

size_t ThreadPool::threadCount() const
{
    return _threads.size();
}

void ThreadPool::notifyThreads()
{
    switch (_queue.size())
    {
    case 0:
        break;
    case 1:
        _cv.notify_one();
        break;
    default:
        _cv.notify_all();
        break;
    }
}

void ThreadPool::ThreadFn(ThreadPool& pool)
{
    for (;;)
    {
        std::unique_lock lock{pool._m};
        if (pool._queue.empty() && pool._isRunning)
            pool._cv.wait(lock);
        const bool needToExit = pool._queue.empty() && !pool._isRunning;
        if (needToExit)
            break;
        if (pool._queue.empty())
            continue;

        auto task = std::move(pool._queue.front());
        pool._queue.pop();
        pool.notifyThreads();
        lock.unlock();
        task->run();
    }
}

}