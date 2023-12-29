#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
private:
    std::chrono::steady_clock::time_point startTime;

public:
    Timer() { reset(); };
    void reset();
    double elapsed() const;
};

#endif
