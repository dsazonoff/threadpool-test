#include "Adder.h"

#include <cassert>

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
    pool.push(std::make_unique<AccumulateTask>(storage, pool));
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
    }

    auto result = fn(v1, v2);
    {
        std::lock_guard lock{_storage.mutex()};
        _storage.pushSum(result);
        if (!_storage.isFinished())
            NumberStorage::launch(_storage, _pool);
    }
}
