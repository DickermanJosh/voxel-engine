#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "ChunkGenerator.hpp"
#include "HashUtils.hpp"
#include "Camera.hpp"

#include <cstdint>
#include <unordered_map>
#include <memory>

class World {
    public:
        static constexpr int VIEW_DISTANCE = 5; // Chunk units
    public:
        Chunk* getChunk(int cx, int cy, int cz);
        void update(Camera& camera, float dt);
        void draw();

        World(uint64_t seed);
        ~World() = default;
    private:
        uint64_t m_Seed;
        ChunkGenerator m_ChunkGenerator;
        std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>> m_Chunks; // Current chunks loaded in memory
};

#endif // WORLD_HPP
