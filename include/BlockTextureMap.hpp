// BlockTextureMap.hpp
#ifndef BLOCKTEXTUREMAP_HPP
#define BLOCKTEXTUREMAP_HPP

#include "BlockType.hpp"
#include <array>
#include <utility>

// 6 faces per block
constexpr int NUM_FACES = 6;
constexpr int UNIQUE_BLOCKS = 3;

// Indexed by BlockType (as integer) and face index
inline const std::array<std::array<std::pair<int, int>, NUM_FACES>, UNIQUE_BLOCKS> blockTextureMap = {{
    // Stone
    {{{3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}}},

        // Grass
        {{{1, 0}, {1, 0}, {1, 0}, {1, 0}, {2, 0}, {0, 0}}},

        // Dirt
        {{{2, 0}, {2, 0}, {2, 0}, {2, 0}, {2, 0}, {2, 0}}}
}};

inline std::pair<int, int> getFaceTile(BlockType type, int faceIndex) {
    return blockTextureMap[static_cast<int>(type) - 1][faceIndex];
}

#endif

