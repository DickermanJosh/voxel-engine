#include "ChunkGenerator.hpp"
#include <iostream>

void ChunkGenerator::populateChunk(Chunk& chunk, int cx, int cy, int cz)
{
    std::cout << "Populating chunk at " << cx << ", " << cy << ", " << cz << std::endl;
    float worldOffsetX = cx * Chunk::kChunkWidth;
    float worldOffsetY = cy * Chunk::kChunkHeight;
    float worldOffsetZ = cz * Chunk::kChunkDepth;

    for (int z = 0; z < Chunk::kChunkDepth; ++z) {
        float worldZ = worldOffsetZ + z;
        for (int x = 0; x < Chunk::kChunkWidth; ++x) {
            float worldX = worldOffsetX + x;

            double noiseVal = m_PerlinNoise.normalizedOctave2D_01(
                    worldX * 0.125,
                    worldZ * 0.125,
                    4,   // octaves
                    0.5  // persistence
                    );

            int terrainHeight = static_cast<int>(noiseVal * (Chunk::kChunkHeight - 1));

            for (int y = 0; y < Chunk::kChunkHeight; ++y) {
                if (y < terrainHeight && y > terrainHeight - 4) {
                    chunk.setBlock(x, y, z, BlockType::Dirt);
                }
                else if (y < terrainHeight) {
                    chunk.setBlock(x, y, z, BlockType::Stone);
                }
                else if (y == terrainHeight) {
                    chunk.setBlock(x, y, z, BlockType::Grass);
                    // chunk.setBlock(x, y, z, BlockType::Air);
                }
                else {
                    chunk.setBlock(x, y, z, BlockType::Air);
                }
            }
        }
    }

    // chunk.generateMesh();
}

