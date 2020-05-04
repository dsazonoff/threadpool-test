#pragma once

namespace core
{

class ThreadPool;

class Task
{
public:
    Task() = default;
    virtual ~Task() = default;

    virtual void run(ThreadPool& pool) = 0;
};

}