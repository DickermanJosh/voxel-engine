#ifndef SPARCE_CHUNK_DATA_HPP
#define SPARCE_CHUNK_DATA_HPP

#include "BlockType.hpp"
#include "HashUtils.hpp"

#include <glm/glm.hpp>
#include <unordered_map>

struct SparseChunkData {
    public:
        void setBlock(int x, int y, int z, BlockType type);
        BlockType getBlock(int x, int y, int z) const;

    private:
        std::unordered_map<glm::ivec3, BlockType> m_Blocks;
};

#endif
