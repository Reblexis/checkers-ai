#include "includes/timer.hpp"
#include <iostream>

Timer::Timer(long long milliseconds)
        : leftTime(milliseconds),
          isRunning(false), lastStartTime(std::chrono::steady_clock::now())
{}

void Timer::update() {
    auto now = std::chrono::steady_clock::now();
    lastStartTime = now;
    if(!isRunning)
        return;
    leftTime -= now - lastStartTime;
}

void Timer::reset(long long milliseconds) {
    leftTime = std::chrono::milliseconds(milliseconds);
    isRunning = false;
}

void Timer::resume() {
    update();
    isRunning = true;
}

void Timer::pause() {
    update();
    isRunning = false;
}

long long Timer::getRemainingTime() const {
    long long remainingTime = leftTime.count();
    if(isRunning)
        remainingTime -= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastStartTime).count();
    return remainingTime;
}

bool Timer::isFinished() const{
    return getRemainingTime() <= 0;
}
