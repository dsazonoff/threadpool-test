#pragma once

#include <atomic>
#include <cassert>
#include <future>

#include "Task.h"
#include "ThreadPool.h"


class CounterTask;

class ICounter
{
    friend class CounterTask;

public:
    virtual ~ICounter() = default;

private:
    virtual void proceed(core::ThreadPool& pool) = 0;
};


class CounterTask
    : public core::Task
{
public:
    explicit CounterTask(ICounter& counter);
    ~CounterTask() override = default;

    void run(core::ThreadPool& pool) override;

private:
    ICounter& _counter;
};


template <typename Container = std::vector<int>>
class AverageCounter
    : public ICounter
{
public:
    using Type = typename Container::value_type;
    using Iterator = typename Container::const_iterator;

public:
    explicit AverageCounter(const Container& container);
    ~AverageCounter() override = default;

    void start(core::ThreadPool& pool);
    std::future<float> result();

private:
    void proceed(core::ThreadPool& pool) override;

private:
    std::promise<float> _result;
    std::mutex _m;
    const Container& _numbers;
    Iterator _it;
    std::atomic<Type> _sum = 0;
    size_t _elementsProcessed = 0;
};


template <typename Container>
AverageCounter<Container>::AverageCounter(const Container& container)
    : _numbers{container}, _it{std::begin(_numbers)}
{
}

template <typename Container>
void AverageCounter<Container>::start(core::ThreadPool& pool)
{
    const auto nInitialTasks = _numbers.size();
    std::vector<std::unique_ptr<CounterTask>> tasks;

    tasks.reserve(nInitialTasks);
    for (auto i = 0; i < nInitialTasks; ++i)
        tasks.emplace_back(std::make_unique<CounterTask>(*this));
    pool.push(tasks.begin(), tasks.end());
}

template <typename Container>
std::future<float> AverageCounter<Container>::result()
{
    return _result.get_future();
}

template <typename Container>
void AverageCounter<Container>::proceed(core::ThreadPool& pool)
{
    (void)pool;
    Iterator it;
    {
        const std::lock_guard lock{_m};
        if (_it == _numbers.cend())
            return;

        it = _it;
        ++_it;
    }

    _sum += *it;

    {
        const std::lock_guard lock{_m};
        ++_elementsProcessed;
        if (_elementsProcessed != _numbers.size())
            return;

        const float result = static_cast<float>(_sum) / _numbers.size();
        _result.set_value(result);
    }
}
