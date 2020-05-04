#include <iostream>
#include <random>

#include "ThreadPool.h"
#include "AverageCounter.h"


template <size_t nThreads = 0>
void run(const std::vector<int>& numbers)
{
    using namespace core;

    AverageCounter counter{numbers};
    ThreadPool threadPool{nThreads};

    counter.start(threadPool);

    auto result = counter.result();
    result.wait();
    std::cout << "Threads: " << threadPool.threadCount()
              << "\nNumbers: " << numbers.size()
              << "\nAverage is: " << result.get() << std::endl << std::endl;
}

template <size_t Count, int Min = 0, int Max = 10000, size_t nThreads = 0>
void runRandom()
{
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution dist{Min, Max};
    std::vector<int> randomData;

    randomData.reserve(Count);
    for (auto i = 0; i < Count; ++i)
        randomData.push_back(dist(gen));

    run(randomData);
}

template <size_t Count, size_t nThreads = 0>
void runLinear()
{
    std::vector<int> data;

    data.reserve(Count);
    for (auto i = 0; i < Count; ++i)
        data.push_back(i+1);

    run(data);
}


int main()
{
    run<>({1, 2, 3, 4, 5,});
    runRandom<1000>();
    runLinear<10000>();

    std::cout << "Done..." << std::endl;
    return 0;
}