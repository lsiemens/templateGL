#ifndef FRAME_TIMER_H
#define FRAME_TIMER_H

#include "looplog.h"

/// An interface for timming the render loop. One of the implementations
/// of `FrameTimer` should be used to calculate time steps in the render
/// loop as well as report the current frame rate and other statistics.
/// In the render loop `timer()` should only be called once to get the
/// to get the current timestep `dt`.
class FrameTimer {
// Call timer() once in the render loop preferably at the begining or end.
public:
    virtual ~FrameTimer() = default;
    /// Calculates the current time step and adds the frame rate to the
    /// `LoopLog` buffer.
    /// @return Time since the previous invocation of `timer()`.
    virtual float timer() = 0;
    /// @return The time since the GLFW window was created.
    virtual float getTime() const = 0;
};

/// This implementation of `FrameTimer` computes the times step and addes
/// only the frame rate to the `LoopLog` buffer.
class BasicTimer : public FrameTimer {
private:
    float time, previous_time, previous_update;
    unsigned int frame_count;
    LoopLog* loopLog;
public:
    BasicTimer();
    float timer() override;
    float getTime() const override;
};

/// This implementation of `FrameTimer` computes the times step and addes
/// the frame rate as well as some statistics about the time step to the
/// `LoopLog` buffer.
class AdvancedTimer : public FrameTimer {
private:
    float time, previous_time, previous_update;
    float min_dt, max_dt, mean_dt, previous_mean_dt;
    float current_M2, previous_M2;

    unsigned int frame_count;
    LoopLog* loopLog;

    /// Reset parameters used to compute Welford's online algorithm
    /// for the variance.
    void resetWelford();
public:
    AdvancedTimer();
    float timer() override;
    float getTime() const override;
};

#endif
