#include <chrono>

#include "Timer.h"

void Timer::reset()
{
    startTime = std::chrono::steady_clock::now();
}

double Timer::elapsed() const
{
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime);
    return (double) duration.count() / 1e6; // Second
}
