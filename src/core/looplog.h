#ifndef LOOPLOG_H
#define LOOPLOG_H

#include <sstream>

/// A class for convienient logging in a render loop.
/// Text added to `instance.log` during the render loop will be printed
/// to the standard output device when `instance.flush()` is called. The
/// intention is that any functions called during the render loop can
/// log info in a live update instead of a stream of text flowing by.
/// LoopLog should only be flushed once per render iteration.
class LoopLog {
private:
    static LoopLog* m_instance;

    LoopLog();

    /// Returns the cursor to where it was before the buffer was flushed
    /// to the standard output.
    void resetCursor();

    LoopLog(const LoopLog&) = delete;
    LoopLog& operator=(const LoopLog&) = delete;
public:
    std::stringstream m_log;

    /// Gets and instance of the LoogLoop singleton.
    /// @return Pointer to an instance of LoogLoop.
    static LoopLog* getInstance();

    /// Writes the log buffer to the standard output. This function should
    /// only be called once in the render loop.
    void flush();
};

#endif
