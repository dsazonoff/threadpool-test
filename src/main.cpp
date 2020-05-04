#include <iostream>
#include <random>

#include "ThreadPool.h"
#include "Adder.h"


void calculateAndPrintResult(NumberStorage& numbers, unsigned int nThreads)
{
    using namespace core;

    ThreadPool threadPool{nThreads};
    NumberStorage::launch(numbers, threadPool);

    auto result = numbers.result();
    result.wait();
    std::cout << "Threads: " << threadPool.threadCount()
              << "\nNumbers: " << numbers.baseAmount()
              << "\nAverage is: " << result.get() << std::endl << std::endl;
}

template <size_t nThreads = 0>
void run(const std::initializer_list<int>& nums)
{
    NumberStorage numbers{nums};
    calculateAndPrintResult(numbers, nThreads);
}

template <size_t Count, int Min = 0, int Max = 10000, size_t nThreads = 0>
void runRandom()
{
    std::mt19937 rd;
    std::uniform_int_distribution dist{Min, Max};
    std::vector<int> randomData;

    randomData.reserve(Count);
    for (auto i = 0; i < Count; ++i)
        randomData.push_back(dist(rd));

    NumberStorage numbers{randomData.begin(), randomData.end()};
    calculateAndPrintResult(numbers, nThreads);
}


int main()
{
    run<1>({1, 2, 3, 4, 5,});
    run<5>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,});
    run<>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,});
    runRandom<1000>();

    std::cout << "Done..." << std::endl;
    return 0;
}