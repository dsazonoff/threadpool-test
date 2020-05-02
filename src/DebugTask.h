#pragma once

#include "Task.h"


namespace core
{

class DebugTask
    : public Task
{
public:
    explicit DebugTask(int num);
    ~DebugTask() = default;

    void run() override;

private:
    int _num;
};

}



