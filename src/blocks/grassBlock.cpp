#include "GrassBlock.hpp"

GrassBlock::GrassBlock(const glm::vec3& position)
    : Block(position, { true, true, true, true, true, true }) {
    // Grass block top face texture coordinates
    std::vector<float> topFace = {
        -1.0f,  1.0f, -1.0f, 0.0f, 0.0f,
        1.0f,  1.0f, -1.0f, 1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
    };

    // Grass block side face texture coordinates
    std::vector<float> sideFace = {
        -1.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        1.0f,  0.0f, -1.0f, 1.0f, 0.0f,
        1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
    };

    // Grass block bottom face texture coordinates
    std::vector<float> bottomFace = {
        -1.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        1.0f,  0.0f, -1.0f, 1.0f, 0.0f,
        1.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  0.0f,  1.0f, 0.0f, 1.0f,
    };
}
