#pragma once
#include <chrono>
#include <stdint.h>

struct BenchmarkData
{
    double megaCycles;
    double miliSec;    
};


uint64_t rdtsc()
{
    unsigned c,d;
    asm volatile("rdtsc" : "=a" (c), "=d" (d)); //assembly code running the instruction rdtsc
    return (((uint64_t)c) | (((uint64_t)d) << 32)); // calculating the tick value.
}


class Benchmark
{
public:
    void Start()
    {
        startTime = std::chrono::high_resolution_clock::now();
        startCycles = rdtsc();
    }
    
    BenchmarkData Measure()
    {
        BenchmarkData output;

        using namespace std::chrono;
        auto endTime = high_resolution_clock::now();
        auto start = time_point_cast<microseconds>(startTime).time_since_epoch();
        auto end = time_point_cast<microseconds>(endTime).time_since_epoch();
        auto duration = end - start;

        uint64_t endCycles = rdtsc();

        output.megaCycles = (endCycles - startCycles) / 1e6;
        output.miliSec = duration.count() * 0.001;

        return output;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    uint64_t startCycles;
};
