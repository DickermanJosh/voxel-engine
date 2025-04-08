#include "GrassBlock.hpp"

GrassBlock::GrassBlock(const glm::vec3& position,
        const std::vector<bool>& visibleFaces)
    : Block(position, { true, true, true, true, true, true }) {
        generateMesh(visibleFaces);
}

std::pair<int, int> GrassBlock::getFaceTile(int faceIndex) const {
    // Face order is: 
    // 0 -> -Z, 1 -> +Z, 2 -> -X, 3 -> +X, 4 -> -Y, 5 -> +Y
    // For grass block:
    //   top (face 5): grass_top => (0,0)
    //   bottom (face 4): dirt => (2,0)
    //   everything else => grass_side => (1,0)
    //
    switch (faceIndex) {
        case 5: // Top
            return {0, 0};
        case 4: // Bottom
            return {2, 0};
        default: // Sides
            return {1, 0};
    }
}
