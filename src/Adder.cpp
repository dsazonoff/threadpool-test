#include "Adder.h"

#include <cassert>
#include <iostream>

#include "ThreadPool.h"


NumberStorage::NumberStorage(const std::initializer_list<int>& numbers)
    : _numbers{numbers}, _baseAmount{numbers.size()}
{
    assert(!_numbers.empty());
}

std::mutex& NumberStorage::mutex()
{
    return _m;
}

bool NumberStorage::isFinished() const
{
    return _baseAmount == _sumsDone + 1;
}

bool NumberStorage::popNumbers(int& v1, int& v2)
{
    if (_numbers.size() < 2)
        return false;

    v1 = _numbers.front();
    _numbers.pop();
    v2 = _numbers.front();
    _numbers.pop();

    return true;
}

void NumberStorage::pushSum(int sum)
{
    _numbers.push(sum);
    _sumsDone++;

    if (isFinished())
    {
        const float result = static_cast<float>(_numbers.front()) / _baseAmount;
        _result.set_value(result);
    }
}

std::future<float> NumberStorage::result()
{
    return _result.get_future();
}

void NumberStorage::launch(NumberStorage& storage, core::ThreadPool& pool)
{
    std::vector<std::unique_ptr<AccumulateTask>> tasks;
    const auto nInitialTasks = storage._baseAmount / 2;
    tasks.reserve(nInitialTasks);
    for (auto i = 0; i < nInitialTasks; ++i)
        tasks.emplace_back(std::make_unique<AccumulateTask>(storage, pool));

    pool.push(tasks.begin(), tasks.end());
}

size_t NumberStorage::numbersLeft() const
{
    return _numbers.size();
}

size_t NumberStorage::baseAmount() const
{
    return _baseAmount;
}


AccumulateTask::AccumulateTask(NumberStorage& numberStorage, core::ThreadPool& pool)
    : _storage{numberStorage}, _pool{pool}
{
}

void AccumulateTask::run()
{
    static auto fn = [](int a, int b)
    {
        return a + b;
    };

    int v1, v2;
    {
        std::lock_guard lock{_storage.mutex()};
        if (!_storage.popNumbers(v1, v2))
            return;

#if false
        // Debug trace
        {
            static std::mutex coutLock;
            std::lock_guard lock{coutLock};
            std::cout << "Thread: " << std::this_thread::get_id() << " Sum: " << v1 << " + " << v2<<  std::endl;
        }
#endif
    }

    auto result = fn(v1, v2);
    bool needNewTask = false;
    {
        std::lock_guard lock{_storage.mutex()};
        _storage.pushSum(result);
        needNewTask = _storage.numbersLeft() >= 2;
    }

    if (needNewTask)
        _pool.push(std::make_unique<AccumulateTask>(_storage, _pool));
}
