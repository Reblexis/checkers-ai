#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

#include <chrono>

/**
 * @brief Class for managing a timer.
 *
 * A timer that can be paused, resumed and reset.
 *
 * @var lastStartTime The time the timer was last started (used for calculating remaining time)
 * @var leftTime The time remaining on the timer
 * @var isRunning Whether the timer is currently running
 */
class Timer {
private:
    std::chrono::steady_clock::time_point lastStartTime;
    mutable std::chrono::steady_clock::duration leftTime;
    bool isRunning;

    void update(); ///< Updates the time remaining on the timer

public:
    explicit Timer(long long milliseconds);

    void reset(long long milliseconds); ///< Resets the timer to the given time
    void resume(); ///< Resumes the timer
    void pause(); ///< Pauses the timer
    long long getRemainingTime() const; ///< Returns the time remaining on the timer
    bool isFinished() const; ///< Returns whether the timer has finished (time remaining is 0 or less)
};

#endif //TIMER_HPP
