#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>

#include <GL/glew.h>
#include "stb_image.h"

GLuint loadTexture(const char *path);

#include <unistd.h>         // for readlink
#include <limits.h>         // for PATH_MAX
#include <filesystem>       // C++17 filesystem support
inline std::filesystem::path getExecutableDir() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count == -1) {
        std::cerr << "Error: could not resolve /proc/self/exe\n";
        return ".";  // fallback to current directory
    }
    return std::filesystem::path(std::string(result, count)).parent_path();
}
#endif // UTILS_HPP
