#include "DebugTask.h"

#include <iostream>
#include <thread>


namespace core
{

DebugTask::DebugTask(int num)
    : _num{num}
{
}

void DebugTask::run()
{
    static std::mutex coutLocker;
    std::lock_guard lock{coutLocker};
    std::cout << "Task: " << _num << " Thread: " << std::this_thread::get_id() << std::endl;
}

}
