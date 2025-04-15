#include "ChunkGenerator.hpp"

void ChunkGenerator::populateChunk(Chunk& chunk, int cx, int cy, int cz)
{
    float worldOffsetX = cx * Chunk::kChunkWidth;
    float worldOffsetZ = cz * Chunk::kChunkDepth;

    for (int z = 0; z < Chunk::kChunkDepth; ++z) {
        for (int x = 0; x < Chunk::kChunkWidth; ++x) {
            float worldX = worldOffsetX + x;
            float worldZ = worldOffsetZ + z;

            double noiseVal = m_PerlinNoise.normalizedOctave2D_01(
                    worldX * 0.1,
                    worldZ * 0.1,
                    4,   // octaves
                    0.5  // persistence
                    );

            int terrainHeight = static_cast<int>(noiseVal * (Chunk::kChunkHeight - 1));

            for (int y = 0; y < Chunk::kChunkHeight; ++y) {
                if (y < terrainHeight) {
                    chunk.setBlock(x, y, z, BlockType::Stone);
                }
                else if (y == terrainHeight) {
                    // chunk.setBlock(x, y, z, BlockType::Grass);
                    chunk.setBlock(x, y, z, BlockType::Air);
                }
                else {
                    chunk.setBlock(x, y, z, BlockType::Air);
                }
            }
        }
    }

    chunk.generateMesh();
}

