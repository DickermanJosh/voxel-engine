#include "ChunkGenerator.hpp"

std::unique_ptr<Chunk> ChunkGenerator::generateChunk(int cx, int cy, int cz) {
    auto newChunk = std::make_unique<Chunk>(
            m_World,
            glm::vec3(cx * Chunk::kChunkWidth,
                cy * Chunk::kChunkHeight,
                cz * Chunk::kChunkDepth));

    populateChunk(*newChunk, cx, cy, cz);

    return newChunk;
}

// TODO: Figure out a better way to streamline air-chunk creation w/o creating chunk obj first
void ChunkGenerator::populateChunk(Chunk& chunk, int cx, int cy, int cz)
{
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
                    chunk.setBlock(x, y, z, BlockType::Stone);
                } else if (worldY < terrainHeight - 1) {
                    chunk.setBlock(x, y, z, BlockType::Dirt);
                } else if (std::abs(worldY - terrainHeight) < 0.1f) {
                    chunk.setBlock(x, y, z, BlockType::Grass);
                } else {
                    chunk.setBlock(x, y, z, BlockType::Air);
                }
            }
        }
    }
}

