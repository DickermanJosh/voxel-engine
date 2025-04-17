#include "ChunkGenerator.hpp"

std::unique_ptr<Chunk> ChunkGenerator::generateChunk(int cx, int cy, int cz) {
    // populate stone canvas
    auto chunk = populateChunk(cx, cy, cz);
    // TODO (in order)
    // add water
    // carve caves
    // add decorations
    // add mobs
    return chunk;
}

std::unique_ptr<Chunk> ChunkGenerator::populateChunk(int cx, int cy, int cz)
{
    // allocate an empty chunk pointer, only create a chunk object if a block gets added to it
    std::unique_ptr<Chunk> chunk = nullptr;
    // std::cout << "Populating chunk at " << cx << ", " << cy << ", " << cz << std::endl;
    float worldOffsetX = cx * Chunk::kChunkWidth;
    float worldOffsetY = cy * Chunk::kChunkHeight;
    float worldOffsetZ = cz * Chunk::kChunkDepth;

    for (int z = 0; z < Chunk::kChunkDepth; ++z) {
        float worldZ = worldOffsetZ + z;
        for (int x = 0; x < Chunk::kChunkWidth; ++x) {
            float worldX = worldOffsetX + x;

            double baseShape = m_PerlinNoise.normalizedOctave2D_01(worldX * 0.002, worldZ * 0.002, 4, 0.5);
            double mountainMask = m_PerlinNoise.normalizedOctave2D_01(worldX * 0.0004, worldZ * 0.0004, 5, 0.4);
            double cliffDetail = m_PerlinNoise.normalizedOctave2D_01(worldX * 0.005, worldZ * 0.005, 6, 0.45);

            double elevation = baseShape * 50.0
                + std::pow(mountainMask, 2.0) * 300.0
                + cliffDetail * 20.0;

            int terrainHeight = static_cast<int>(elevation);

            for (int y = 0; y < Chunk::kChunkHeight; ++y) {
                float worldY = worldOffsetY + y;

                if (worldY < terrainHeight - 4) {
                    setBlockAtChunkPos(chunk, BlockType::Stone, cx, cy, cz, x, y, z);
                } else if (worldY < terrainHeight - 1) {
                    setBlockAtChunkPos(chunk, BlockType::Dirt, cx, cy, cz, x, y, z);
                } else if (std::abs(worldY - terrainHeight) < 0.1f) {
                    setBlockAtChunkPos(chunk, BlockType::Grass, cx, cy, cz, x, y, z);
                } else {
                    if (chunk != nullptr) chunk->setBlock(x, y, z, BlockType::Air);
                }
            }
        }
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

