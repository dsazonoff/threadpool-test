#include "AverageCounter.h"


CounterTask::CounterTask(ICounter& counter)
    : _counter{counter}
{
}

void CounterTask::run(core::ThreadPool& pool)
{
    _counter.proceed(pool);
}
