#ifndef HASHUTILS_HPP
#define HASHUTILS_HPP

#include <glm/glm.hpp>
#include <functional>
namespace std {
    template<>
    struct hash<glm::ivec3> {
        std::size_t operator()(const glm::ivec3& v) const noexcept {
            std::size_t h1 = std::hash<int>{}(v.x);
            std::size_t h2 = std::hash<int>{}(v.y);
            std::size_t h3 = std::hash<int>{}(v.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

#endif // HASHUTILS_HPP
