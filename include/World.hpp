#ifndef WORLD_HPP
#define WORLD_HPP

#include <Chunk.hpp>
#include "HashUtils.hpp"

#include <cstdint>
#include <unordered_map>
#include <memory>

class World {
    public:
        Chunk* getChunk(int cx, int cy, int cz);
        void update(const glm::vec3& cameraPos, float dt);

        World(uint64_t seed);
        ~World();
    private:
        uint64_t m_Seed;
        std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>> m_Chunks; // Current chunks loaded in memory
};

#endif // WORLD_HPP
