#include "StoneBlock.hpp"

StoneBlock::StoneBlock(const glm::vec3& position,
        const std::vector<bool>& visibleFaces)
    : Block(position, { true, true, true, true, true, true }) {
        generateMesh(visibleFaces);
}

std::pair<int, int> StoneBlock::getFaceTile(int faceIndex) const {
    // Face order is: 
    // 0 -> -Z, 1 -> +Z, 2 -> -X, 3 -> +X, 4 -> -Y, 5 -> +Y
    // For stone block:
    //   everything => (3,0)
    //
    return {3, 0};
}
