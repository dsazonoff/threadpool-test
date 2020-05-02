#pragma once

namespace core
{

class Task
{
public:
    Task() = default;
    virtual ~Task() = default;

    virtual void run() = 0;
};

}