#ifndef CHUNK_GENERATOR_HPP
#define CHUNK_GENERATOR_HPP

#include "Chunk.hpp"
#include "PerlinNoise.hpp" // siv::PerlinNoise

#include <memory>

class ChunkGenerator {
public:
    std::unique_ptr<Chunk> generateChunk(int cx, int cy, int cz);

    explicit ChunkGenerator(World* world, uint64_t seed)
        : m_World(world),
        m_PerlinNoise(static_cast<siv::PerlinNoise::seed_type>(seed)) {}

private:
    World* m_World;
private:
    // Generate the blocks in the chunk, based on chunk coords (cx, cy, cz)
    std::unique_ptr<Chunk> populateChunk(int cx, int cy, int cz);
    // Will instantiate a chunk if std::unique_ptr<Chunk>& chunk == nullptr. Sets the block at that chunks local coords
    void setBlockAtChunkPos(std::unique_ptr<Chunk>& chunk, BlockType block, int chunkX, int chunkY, int chunkZ, int localX, int localY, int localZ);
    siv::PerlinNoise m_PerlinNoise;
};

#endif // CHUNK_GENERATOR_HPP

