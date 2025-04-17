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
    void populateChunk(Chunk& chunk, int cx, int cy, int cz);
    siv::PerlinNoise m_PerlinNoise;
};

#endif // CHUNK_GENERATOR_HPP

