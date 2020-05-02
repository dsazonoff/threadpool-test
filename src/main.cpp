#include <iostream>

#include "ThreadPool.h"
#include "Adder.h"

template<size_t nThreads=0>
void run(const std::initializer_list<int>& nums)
{
    using namespace core;

    NumberStorage numbers{nums};
    ThreadPool threadPool{nThreads};
    NumberStorage::launch(numbers, threadPool);

    auto result = numbers.result();
    result.wait();
    std::cout << "Threads: " << nThreads << "\nAverage is: " << result.get() << "\n\n";
}

int main()
{
    run<1>({1, 2, 3, 4, 5, });
    run<5>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, });
    run<>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, });

    std::cout << "Done..." << std::endl;
    return 0;
}