#include "ChunkGenerator.hpp"

std::unique_ptr<Chunk> ChunkGenerator::generateChunk(int cx, int cy, int cz) {
    // populate stone canvas
    auto chunk = populateChunk(cx, cy, cz);
    // TODO (in order)
    // define biome
    // add water
    // add ore
    // carve caves
    // add surface decorations
    // add mobs
    return chunk;
}

std::unique_ptr<Chunk> ChunkGenerator::populateChunk(int cx, int cy, int cz)
{
    struct BlockSet {
        int x, y, z;
        BlockType type;
    };

    std::vector<BlockSet> placedBlocks;
    placedBlocks.reserve(Chunk::kChunkWidth * Chunk::kChunkHeight * Chunk::kChunkDepth);

    float worldOffsetX = cx * Chunk::kChunkWidth;
    float worldOffsetY = cy * Chunk::kChunkHeight;
    float worldOffsetZ = cz * Chunk::kChunkDepth;

    // Iterate through each block in the chunk and store its info based on noise maps
    for (int z = 0; z < Chunk::kChunkDepth; ++z) {
        float worldZ = worldOffsetZ + z;
        for (int x = 0; x < Chunk::kChunkWidth; ++x) {
            float worldX = worldOffsetX + x;

            int terrainHeight = getHeight(worldX, worldZ);

            for (int y = 0; y < Chunk::kChunkHeight; ++y) {
                float worldY = worldOffsetY + y;
                BlockType type = BlockType::Air;

                if (worldY < terrainHeight - 4)
                    type = BlockType::Stone;
                else if (worldY < terrainHeight - 1)
                    type = BlockType::Dirt;
                else if (std::abs(worldY - terrainHeight) < 0.1f)
                    type = BlockType::Grass;

                if (type != BlockType::Air) {
                    placedBlocks.push_back({x, y, z, type});
                }
            }
        }
    }

    if (placedBlocks.empty()) {
        return nullptr; // all air
    }

    // Determine what kind of chunk we should use based on how many blocks it contains
    float fillRatio = static_cast<float>(placedBlocks.size()) /
                      (Chunk::kChunkWidth * Chunk::kChunkHeight * Chunk::kChunkDepth);

    Chunk::StorageMode mode = (fillRatio < 0.1f)
        ? Chunk::StorageMode::Sparse
        : Chunk::StorageMode::Dense;

    // Create the new chunk
    auto chunk = std::make_unique<Chunk>(
        m_World,
        glm::vec3(cx * Chunk::kChunkWidth,
                  cy * Chunk::kChunkHeight,
                  cz * Chunk::kChunkDepth),
        mode);

    // Set the blocks in the new chunk from our list created earlier
    for (const auto& b : placedBlocks) {
        chunk->setBlock(b.x, b.y, b.z, b.type);
    }

    return chunk;
}


void ChunkGenerator::setBlockAtChunkPos(std::unique_ptr<Chunk>& chunk,
        BlockType block,
        int chunkX, int chunkY, int chunkZ,
        int localX, int localY, int localZ) {
    if (!chunk) {
        chunk = std::make_unique<Chunk>(
                m_World,
                glm::vec3(chunkX * Chunk::kChunkWidth,
                    chunkY * Chunk::kChunkHeight,
                    chunkZ * Chunk::kChunkDepth));
    }

    chunk->setBlock(localX, localY, localZ, block);
}

bool ChunkGenerator::isChunkEmpty(int cx, int cy, int cz) const {
    float worldOffsetX = cx * Chunk::kChunkWidth;
    float worldOffsetY = cy * Chunk::kChunkHeight;
    float worldOffsetZ = cz * Chunk::kChunkDepth;

    for (int z = 0; z < Chunk::kChunkDepth; ++z) {
        float worldZ = worldOffsetZ + z;
        for (int x = 0; x < Chunk::kChunkWidth; ++x) {
            float worldX = worldOffsetX + x;

            int terrainHeight = getHeight(worldX, worldZ);

            float maxBlockY = worldOffsetY + Chunk::kChunkHeight;

            // If this chunk is at or below terrainHeight, it's not all air
            if (maxBlockY > terrainHeight - 4) {
                return false;
            }
        }
    }

    return true; // Nothing here intersects terrain
}

int ChunkGenerator::getHeight(int worldX, int worldZ) const {
            double baseShape = m_PerlinNoise.normalizedOctave2D_01(worldX * 0.002, worldZ * 0.002, 4, 0.5);
            double mountainMask = m_PerlinNoise.normalizedOctave2D_01(worldX * 0.0004, worldZ * 0.0004, 5, 0.4);
            double cliffDetail = m_PerlinNoise.normalizedOctave2D_01(worldX * 0.005, worldZ * 0.005, 6, 0.45);

            double elevation = baseShape * 50.0
                + std::pow(mountainMask, 2.0) * 300.0
                + cliffDetail * 20.0;

            // final terrain height
            return static_cast<int>(elevation);
}

