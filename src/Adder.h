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
    ~NumberStorage() = default;

    std::mutex& mutex();
    bool isFinished() const;
    std::future<float> result();

    bool popNumbers(int& v1, int& v2);
    void pushSum(int sum);

public:
    static void launch(NumberStorage& storage, core::ThreadPool& pool);

private:
    std::promise<float> _result;
    std::queue<int> _numbers;
    const size_t _baseAmount;
    size_t _sumsDone = 0;
    std::mutex _m;
};


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
