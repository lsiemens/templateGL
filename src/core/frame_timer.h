#ifndef FRAME_TIMER_H
#define FRAME_TIMER_H

#include "core/looplog.h"

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
    virtual double timer() = 0;
    /// @return The time since the GLFW window was created.
    virtual double getTime() const = 0;
};

/// This implementation of `FrameTimer` computes the times step and addes
/// only the frame rate to the `LoopLog` buffer.
class BasicTimer : public FrameTimer {
private:
    double m_time, m_previous_time, m_previous_update;
    unsigned int m_frame_count;
    LoopLog* m_loopLog;
public:
    BasicTimer();
    double timer() override;
    double getTime() const override;
};

/// This implementation of `FrameTimer` computes the times step and addes
/// the frame rate as well as some statistics about the time step to the
/// `LoopLog` buffer.
class AdvancedTimer : public FrameTimer {
private:
    double m_time, m_previous_time, m_previous_update;
    double m_min_dt, m_max_dt, m_mean_dt, m_previous_mean_dt;
    double m_current_M2, m_previous_M2;

    unsigned int m_frame_count;
    LoopLog* m_loopLog;

    /// Reset parameters used to compute Welford's online algorithm
    /// for the variance.
    void resetWelford();
public:
    AdvancedTimer();
    double timer() override;
    double getTime() const override;
};

#endif
