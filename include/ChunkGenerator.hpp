#ifndef CHUNK_GENERATOR_HPP
#define CHUNK_GENERATOR_HPP

#include "Chunk.hpp"
#include "PerlinNoise.hpp" // siv::PerlinNoise

class ChunkGenerator {
public:
    explicit ChunkGenerator(uint64_t seed)
        : m_PerlinNoise(static_cast<siv::PerlinNoise::seed_type>(seed)) {}

    // Generate the blocks in the chunk, based on chunk coords (cx, cy, cz)
    void populateChunk(Chunk& chunk, int cx, int cy, int cz);

private:
    siv::PerlinNoise m_PerlinNoise;
};

#endif // CHUNK_GENERATOR_HPP

