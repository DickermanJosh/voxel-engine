#ifndef WORLD_HPP
#define WORLD_HPP

#include "ChunkGenerator.hpp"
#include "HashUtils.hpp"
#include "Player.hpp"
#include "Chunk.hpp"
#include "Utils.hpp"

#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <memory>
#include <queue>
#include <mutex>

class World {
    public:
        static constexpr int VIEW_DISTANCE = 12; // Chunk units
    public:
        // Takes in an ivec3 world position and returns the type of block that is present
        BlockType getBlockAtWorld(const glm::ivec3& worldPos) const;
        // Takes in an ivec3 world position and returns the chunk at that pos
        // Returns nullptr if no chunk is present
        Chunk* getChunkAtWorld(const glm::ivec3& worldPos) const;
        Chunk* getChunkAtChunkPos(const glm::ivec3& chunkPos) const;
        // Takes in coordinates in chunk space (world space / chunk sizes)
        // Returns the chunk present in that pos, calls for its generation if it
        // does not exist
        Chunk* getChunk(int cx, int cy, int cz);
        Player* getPlayer(); // m_Player getter
        // Takes in ivec3 chunk position and adds that chunk to the rendering queue
        void queueChunkForRemeshing(const glm::ivec3& pos);
        // update is called each frame
        void update(float dt);
        void draw();

        World(uint64_t seed);
        ~World() = default;
    private:
        uint64_t m_Seed;
        ChunkGenerator m_ChunkGenerator;
        Player m_Player;
        glm::ivec3 m_LastKnownPlayerChunk;
        std::queue<glm::ivec3> m_ChunkGenQueue; // chunk generation queue
        // std::priority_queue<glm::ivec3, std::vector<glm::ivec3>, ChunkDistanceComparator> m_ChunkGenQueue;
        mutable std::mutex m_ChunkGenQueueMutex;
        std::unordered_set<glm::ivec3> m_QueuedChunks; // prevent duplicates in the generation queue

        std::queue<glm::ivec3> m_MeshQueue; // chunk mesh generation queue
        mutable std::mutex m_MeshQueueMutex;
        std::unordered_set<glm::ivec3> m_MeshQueuedChunks; // prevent duplicates in the mesh queue

        std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>> m_Chunks; // Current chunks loaded in memory
        mutable std::mutex m_ChunkMutex;
        std::vector<glm::ivec3> m_AirChunks; // List the chunks found containing only air so we don't try to re-load them
        constexpr static float UNLOAD_INTERVAL = 0.5f; // Interval for unloading outdated chunks in the update loop
        float m_UnloadTimer = 0.0f;
    private:
        glm::ivec3 worldToChunkCoords(const glm::vec3& position) const;
        bool isChunkInView(const glm::ivec3& playerChunk, const glm::ivec3& chunkCoords) const;
        void markChunkFaceDirty(const glm::ivec3& chunkCoord, int faceIndex);
        void unloadOutdatedChunks(const glm::ivec3& playerChunkPos);
        void enqueueNearbyChunks(const glm::ivec3& playerChunkPos);
        void sortMeshingQueue(const glm::ivec3& playerChunkPos);
};

#endif // WORLD_HPP
