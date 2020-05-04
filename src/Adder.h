#pragma once

#include <atomic>
#include <queue>
#include <mutex>
#include <initializer_list>
#include <future>

#include "Task.h"


namespace core
{
class ThreadPool;
}


class NumberStorage
{
public:
    NumberStorage(const std::initializer_list<int>& numbers = {});
    template <typename It>
    NumberStorage(const It& begin, const It& end);
    ~NumberStorage() = default;

    std::mutex& mutex();
    bool isFinished() const;
    std::future<float> result();

    bool popNumbers(int& v1, int& v2);
    void pushSum(int sum);
    size_t numbersLeft() const;
    size_t baseAmount() const;

public:
    static void launch(NumberStorage& storage, core::ThreadPool& pool);

private:
    std::promise<float> _result;
    std::queue<int> _numbers;
    const size_t _baseAmount;
    size_t _sumsDone = 0;
    std::mutex _m;
};

template <typename It>
NumberStorage::NumberStorage(const It& begin, const It& end)
    : _baseAmount{static_cast<size_t>(std::distance(begin, end))}
{
    for(auto it = begin; it != end; ++it)
        _numbers.push(*it);
}


class AccumulateTask
    : public core::Task
{
public:
    explicit AccumulateTask(NumberStorage& numberStorage, core::ThreadPool& pool);
    ~AccumulateTask() = default;

    void run() override;

private:
    NumberStorage& _storage;
    core::ThreadPool& _pool;
};
