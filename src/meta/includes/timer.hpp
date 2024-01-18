#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

#include <chrono>

class Timer {
private:
    std::chrono::steady_clock::time_point lastStartTime;
    mutable std::chrono::steady_clock::duration leftTime;
    bool isRunning;

    void update();

public:
    explicit Timer(long long milliseconds);

    void reset(long long milliseconds);
    void resume();
    void pause();
    long long getRemainingTime() const;
    bool isFinished() const;
};

#endif //TIMER_HPP
