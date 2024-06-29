#include "looplog.h"

#include <iostream>

LoopLog* LoopLog::instance = nullptr;

LoopLog::LoopLog() {
}

/// Moves the cursor back to where it was prior to flushing the log.
void LoopLog::resetCursor() {
    std::string buffer = log.str();
    for (char &c : buffer) {
        if (c == '\n') {
            std::cout << "\033[A";
        }
    }
}

LoopLog* LoopLog::getInstance() {
    if (instance == nullptr) {
        instance = new LoopLog();
    }
    return instance;
}

void LoopLog::flush(){
    std::string buffer = log.str();
    if (buffer != "") {
        std::cout << buffer;
        resetCursor();
        std::cout.flush();
    }
    log.str("");
    log.clear();
}
