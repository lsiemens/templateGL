#include "core/frame_timer.h"

#include <cmath>
#include <GLFW/glfw3.h>

#include "core/looplog.h"

BasicTimer::BasicTimer() {
    time = glfwGetTime();
    previous_time = previous_update = time;
    frame_count = 0;

    loopLog = LoopLog::getInstance();
}

float BasicTimer::timer() {
    frame_count++;
    previous_time = time;
    time = glfwGetTime();

    if (time - previous_update >= 1.0f) {
        loopLog->log << "FPS: " << static_cast<float>(frame_count)/(time - previous_update) << " Δt (in ms) : " << (time - previous_update)/static_cast<float>(frame_count) << "\n";
        previous_update = time;
        frame_count = 0;
    }

    return time - previous_time;
}

float BasicTimer::getTime() const {
    return time;
}

/// Reset parameters needed for walford's online algorithm for the variance
/// aswell as limits for finding the minimum and maximum time step.
void AdvancedTimer::resetWelford() {
    min_dt = INFINITY, max_dt = -INFINITY, mean_dt = previous_mean_dt = 0;
    current_M2 = previous_M2 = 0;
    frame_count = 0;
}

AdvancedTimer::AdvancedTimer() {
    time = glfwGetTime();
    previous_time = previous_update = time;
    resetWelford();

    loopLog = LoopLog::getInstance();
}

float AdvancedTimer::timer() {
    frame_count++;
    previous_time = time;
    previous_mean_dt = mean_dt;
    previous_M2 = current_M2;
    time = glfwGetTime();

    //Welford' online algorith for variance
    float dt = time - previous_time;
    mean_dt = previous_mean_dt + (dt - previous_mean_dt)/frame_count;
    current_M2 = previous_M2 + (dt - previous_mean_dt)*(dt - mean_dt);

    if (dt < min_dt) {
        min_dt = dt;
    }
    if (dt > max_dt) {
        max_dt = dt;
    }

    if (time - previous_update >= 1.0f) {
        loopLog->log << "FPS: " << static_cast<float>(frame_count)/(time - previous_update) << "\n";
        loopLog->log << "Δt (in ms) : " << 1000*mean_dt << " ± " << 1000*std::sqrt(current_M2/(frame_count - 1))  << " [Min|Max]: [" << 1000*min_dt << " | " << 1000*max_dt << "]\n";
        previous_update = time;
        resetWelford();
    }

    return time - previous_time;
}

float AdvancedTimer::getTime() const {
    return time;
}
