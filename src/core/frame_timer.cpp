#include "core/frame_timer.h"

#include <cmath>
#include <GLFW/glfw3.h>

#include "core/looplog.h"

BasicTimer::BasicTimer() {
    m_time = glfwGetTime();
    m_previous_time = m_previous_update = m_time;
    m_frame_count = 0;

    m_loopLog = LoopLog::getInstance();
}

double BasicTimer::timer() {
    m_frame_count++;
    m_previous_time = m_time;
    m_time = glfwGetTime();

    if (m_time - m_previous_update >= 1.0f) {
        m_loopLog->m_log << "FPS: " << static_cast<double>(m_frame_count)/(m_time - m_previous_update) << " Δt (in ms) : " << (m_time - m_previous_update)/static_cast<double>(m_frame_count) << "\n";
        m_previous_update = m_time;
        m_frame_count = 0;
    }

    return m_time - m_previous_time;
}

double BasicTimer::getTime() const {
    return m_time;
}

/// Reset parameters needed for walford's online algorithm for the variance
/// aswell as limits for finding the minimum and maximum time step.
void AdvancedTimer::resetWelford() {
    m_min_dt = INFINITY, m_max_dt = -INFINITY, m_mean_dt = m_previous_mean_dt = 0;
    m_current_M2 = m_previous_M2 = 0;
    m_frame_count = 0;
}

AdvancedTimer::AdvancedTimer() {
    m_time = glfwGetTime();
    m_previous_time = m_previous_update = m_time;
    resetWelford();

    m_loopLog = LoopLog::getInstance();
}

double AdvancedTimer::timer() {
    m_frame_count++;
    m_previous_time = m_time;
    m_previous_mean_dt = m_mean_dt;
    m_previous_M2 = m_current_M2;
    m_time = glfwGetTime();

    //Welford' online algorith for variance
    double dt = m_time - m_previous_time;
    m_mean_dt = m_previous_mean_dt + (dt - m_previous_mean_dt)/m_frame_count;
    m_current_M2 = m_previous_M2 + (dt - m_previous_mean_dt)*(dt - m_mean_dt);

    if (dt < m_min_dt) {
        m_min_dt = dt;
    }
    if (dt > m_max_dt) {
        m_max_dt = dt;
    }

    if (m_time - m_previous_update >= 1.0f) {
        m_loopLog->m_log << "FPS: " << static_cast<double>(m_frame_count)/(m_time - m_previous_update) << "\n";
        m_loopLog->m_log << "Δt (in ms) : " << 1000*m_mean_dt << " ± " << 1000*std::sqrt(m_current_M2/(m_frame_count - 1))  << " [Min|Max]: [" << 1000*m_min_dt << " | " << 1000*m_max_dt << "]\n";
        m_previous_update = m_time;
        resetWelford();
    }

    return m_time - m_previous_time;
}

double AdvancedTimer::getTime() const {
    return m_time;
}
