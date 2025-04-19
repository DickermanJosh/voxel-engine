#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "ChunkGenerator.hpp"
#include "HashUtils.hpp"
#include "Player.hpp"

#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <memory>
#include <queue>

class World {
    public:
        static constexpr int VIEW_DISTANCE = 15; // Chunk units
    public:
        BlockType getBlockAtWorld(const glm::ivec3& worldPos) const;
        Chunk* getChunk(int cx, int cy, int cz);
        Player* getPlayer();
        void update(float dt);
        void draw();

        World(uint64_t seed);
        ~World() = default;
    private:
        uint64_t m_Seed;
        ChunkGenerator m_ChunkGenerator;
        Player m_Player;
        glm::ivec3 m_LastKnownPlayerChunk;
        std::queue<glm::ivec3> m_ChunkGenQueue;
        std::queue<glm::ivec3> m_ChunkMeshGenerationQueue;
        std::queue<glm::ivec3> m_MeshQueue;
        std::unordered_set<glm::ivec3> m_QueuedChunks;
        std::unordered_set<glm::ivec3> m_QueuedMeshGenChunks;
        std::unordered_set<glm::ivec3> m_MeshQueuedChunks;
        std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>> m_Chunks; // Current chunks loaded in memory
        std::vector<glm::ivec3> m_AirChunks; // List the chunks found containing only air so we don't try to re-load them
    private:
        glm::ivec3 worldToChunkCoords(const glm::vec3& position) const;
        bool isChunkInView(const glm::ivec3& playerChunk, const glm::ivec3& chunkCoords) const;
        void markChunkFaceDirty(const glm::ivec3& chunkCoord, int faceIndex);
};

#endif // WORLD_HPP
