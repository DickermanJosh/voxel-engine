#include "SparseChunkData.hpp"

void SparseChunkData::setBlock(int x, int y, int z, BlockType type) {
    glm::ivec3 pos(x, y, z);
    if (type == BlockType::Air) {
        m_Blocks.erase(pos);
        return;
    }

    m_Blocks[pos] = type;
}

BlockType SparseChunkData::getBlock(int x, int y, int z) const {
    glm::ivec3 pos(x, y, z);
    auto it = m_Blocks.find(pos);
    return it != m_Blocks.end() ? it->second : BlockType::Air;
}
