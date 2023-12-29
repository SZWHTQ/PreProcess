#include <chrono>

#include "Timer.h"

void Timer::reset()
{
    startTime = std::chrono::steady_clock::now();
}

double Timer::elapsed() const
{
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
    return duration.count() / 1000.0; // Second
}
