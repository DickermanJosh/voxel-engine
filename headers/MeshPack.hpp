#ifndef MESHPACK_HPP
#define MESHPACK_HPP

#include <vector>

typedef struct MeshPack {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
} MeshPack;

#endif // MESHPACK_HPP
