#ifndef PATH_UTIL_H
#define PATH_UTIL_H

#include <unistd.h>
#include <filesystem>
#include <stdexcept>

#define MAX_BUFFER 1024

inline std::filesystem::path executable_dir() {
    char buffer[MAX_BUFFER] = {0};
    // limit to sizeof(buffer) - 1 to leave room for a '\0' at the end
    const ssize_t len = ::readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len <= 0) {
        throw std::runtime_error("Failed to get path of executable");
    }
    buffer[len] = '\0';
    if (len >= MAX_BUFFER - 1) {
        throw std::runtime_error("Path to executable is longer than the limit, MAX_BUFFER == " + std::to_string(MAX_BUFFER));
    }
    return std::filesystem::path(buffer).parent_path();
}

inline std::filesystem::path get_fixed_path(const std::string& rpath) {
    return (executable_dir() / rpath).lexically_normal();
}



#endif
