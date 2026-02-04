#include "core/looplog.h"

#include <iostream>

LoopLog* LoopLog::m_instance = nullptr;

LoopLog::LoopLog() {
}

/// Moves the cursor back to where it was prior to flushing the log.
void LoopLog::resetCursor() {
    std::string buffer = m_log.str();
    for (char &c : buffer) {
        if (c == '\n') {
            std::cout << "\033[A";
        }
    }
}

LoopLog* LoopLog::getInstance() {
    if (m_instance == nullptr) {
        m_instance = new LoopLog();
    }
    return m_instance;
}

void LoopLog::flush(){
    std::string buffer = m_log.str();
    if (buffer != "") {
        std::cout << buffer;
        resetCursor();
        std::cout.flush();
    }
    m_log.str("");
    m_log.clear();
}
