#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
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

#include <random>
#include <cstdint>
inline uint64_t generate_uint64_t() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> distrib(0, std::numeric_limits<uint64_t>::max());
    return distrib(gen);
}

inline auto manhattanDistSq = [](const glm::ivec3& a, const glm::ivec3& b) {
    glm::ivec3 d = a - b;
    return d.x * d.x + d.y * d.y + d.z * d.z;
};

#endif // UTILS_HPP
