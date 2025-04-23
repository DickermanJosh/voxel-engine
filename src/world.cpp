#include "World.hpp"
#include "ChunkGenerator.hpp"
#include <iostream>
#include <ThreadPool.hpp>

World::World(uint64_t seed) 
    : m_Seed(seed), 
    m_ChunkGenerator(this, seed),
    m_Player(glm::vec3(0.0f, 150.0f, 0.0f)),
    m_LastKnownPlayerChunk(worldToChunkCoords(m_Player.getPosition())) { 
        std::cout << "World init with seed: " << seed << std::endl;
        enqueueNearbyChunks(m_LastKnownPlayerChunk);
    };



int maxPerFrame = 4;
void World::update(float dt) {
    m_UnloadTimer += dt;
    m_Player.update(dt);

    glm::ivec3 playerChunk = worldToChunkCoords(m_Player.getPosition());
    bool playerChangedChunks = playerChunk != m_LastKnownPlayerChunk;

    unloadOutdatedChunks(playerChunk);

    if (playerChangedChunks) {
        m_LastKnownPlayerChunk = worldToChunkCoords(m_Player.getPosition());
        ThreadPool::instance().enqueue([this, playerChunk]() {
            enqueueNearbyChunks(playerChunk);
            sortMeshingQueue(playerChunk);
        });
    }

    for (int i = 0; i < maxPerFrame; i++) {
        glm::ivec3 coords;
        {
            std::lock_guard<std::mutex> lock(m_ChunkGenQueueMutex);
            if (m_ChunkGenQueue.empty()) break;
            coords = m_ChunkGenQueue.front();
            m_ChunkGenQueue.pop();
            m_QueuedChunks.erase(coords);
        }

        Chunk* c = getChunk(coords.x, coords.y, coords.z);
        if (c) {
            std::lock_guard<std::mutex> lock(m_MeshQueueMutex);
            if (m_MeshQueuedChunks.insert(coords).second)
                m_MeshQueue.push(coords);
        }
    }

    for (int i = 0; i < maxPerFrame; i++) {
        glm::ivec3 pos;
        {
            std::lock_guard<std::mutex> lock(m_MeshQueueMutex);
            if (m_MeshQueue.empty()) break;
            pos = m_MeshQueue.front();
            m_MeshQueue.pop();
            m_MeshQueuedChunks.erase(pos);
        }

        Chunk* c = getChunkAtChunkPos(pos);
        if (!c) continue;

        c->generateMesh();

        // Regenerate neighbor meshes (if not queued already)
        for (int f = 0; f < 6; f++) {
            glm::ivec3 neighborPos = pos + Chunk::neighborOffsets[f];
            if (m_MeshQueuedChunks.find(neighborPos) == m_MeshQueuedChunks.end()) {
                Chunk* n = getChunkAtChunkPos(neighborPos);
                if (!n) continue;
                std::lock_guard<std::mutex> lock(m_MeshQueueMutex);
                if (m_MeshQueuedChunks.insert(neighborPos).second) {
                    m_MeshQueue.push(neighborPos);
                }
            }
        }
    }

    std::cout << "Loaded Chunks: " << m_Chunks.size() << std::endl;
}

void World::enqueueNearbyChunks(const glm::ivec3& playerChunkPos) {
    std::vector<glm::ivec3> candidates;
    std::unordered_set<glm::ivec3> visibleNow;

    for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
        for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; ++dy) {
            for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
                glm::ivec3 pos = playerChunkPos + glm::ivec3(dx, dy, dz);
                visibleNow.insert(pos);
                {
                    std::scoped_lock lock(m_ChunkMutex, m_ChunkGenQueueMutex);
                    if (m_Chunks.find(pos) == m_Chunks.end() &&
                        m_QueuedChunks.find(pos) == m_QueuedChunks.end()) {
                        candidates.push_back(pos);
                    }
                }
            }
        }
    }

    // Sort the visible chunks by their distance to the player. Closer chunks first
    /*std::sort(candidates.begin(), candidates.end(), [&](const glm::ivec3& a, const glm::ivec3& b) {
            return manhattanDistSq(a, playerChunkPos) < manhattanDistSq(b, playerChunkPos);
            });*/

   std::scoped_lock lock(m_ChunkMutex, m_ChunkGenQueueMutex);
    for (const auto& pos : candidates) {
        if (m_Chunks.find(pos) == m_Chunks.end() &&
            m_QueuedChunks.find(pos) == m_QueuedChunks.end()) {
            m_ChunkGenQueue.push(pos);
            m_QueuedChunks.insert(pos);
        }
    }

    std::queue<glm::ivec3> filteredQueue;
    std::vector<glm::ivec3> filteredList;
    while (!m_ChunkGenQueue.empty()) {
        glm::ivec3 pos = m_ChunkGenQueue.front();
        m_ChunkGenQueue.pop();
        if (visibleNow.count(pos)) {
            // filteredQueue.push(pos);
            filteredList.push_back(pos);
        } else {
            m_QueuedChunks.erase(pos);
        }
    }

    std::sort(filteredList.begin(), filteredList.end(), [&](const glm::ivec3& a, const glm::ivec3& b) {
            return manhattanDistSq(a, playerChunkPos) < manhattanDistSq(b, playerChunkPos);
            });

    for (size_t i = 0; i < filteredList.size(); i++) {
        filteredQueue.push(filteredList[i]);
    }

    filteredList.clear();
    m_ChunkGenQueue = std::move(filteredQueue);
}

void World::sortMeshingQueue(const glm::ivec3& playerChunkPos) {
    std::queue<glm::ivec3> filteredQueue;
    std::vector<glm::ivec3> filteredList;
    std::scoped_lock<std::mutex> lock(m_MeshQueueMutex);

    while (!m_MeshQueue.empty()) {
        glm::ivec3 pos = m_MeshQueue.front();
        m_MeshQueue.pop();
        filteredList.push_back(pos);
    }

    std::sort(filteredList.begin(), filteredList.end(), [&](const glm::ivec3& a, const glm::ivec3& b) {
            return manhattanDistSq(a, playerChunkPos) < manhattanDistSq(b, playerChunkPos);
            });

    for (size_t i = 0; i < filteredList.size(); i++) {
        filteredQueue.push(filteredList[i]);
    }

    filteredList.clear();
    m_MeshQueue = std::move(filteredQueue);

}

Chunk* World::getChunk(int cx, int cy, int cz) {
    glm::ivec3 key(cx, cy, cz);

    {
        std::lock_guard<std::mutex> lock(m_ChunkMutex);
        // first search known air chunks
        for (size_t i = 0; i < m_AirChunks.size(); i++) {
            if (key == m_AirChunks[i]) return nullptr;
        }

        // now search cached chunks
        auto it = m_Chunks.find(key);
        if (it != m_Chunks.end()) {
            // Already exists
            return it->second.get();
        }
    }

    if (m_ChunkGenerator.isChunkEmpty(cx, cy, cz)) {
        std::lock_guard<std::mutex> lock(m_ChunkMutex);
        m_AirChunks.push_back(key);
        return nullptr;
    }

    // Create and generate a new chunk
    auto newChunk = m_ChunkGenerator.generateChunk(cx, cy, cz);

    // If the chunk only contains air mark it for future reference
    if (newChunk == nullptr) {
        std::lock_guard<std::mutex> lock(m_ChunkMutex);
        m_AirChunks.push_back(key);
        return nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(m_ChunkMutex);
        // Store valid chunks in the chunk map
        Chunk* chunkPtr = newChunk.get();
        m_Chunks[key] = std::move(newChunk);
        return chunkPtr;
    }

    // std::cout << "Chunks map size: "<< m_Chunks.size() << std::endl;

    // Ask all neighboring chunks to re-check their meshing on the face
    // facing toward this chunk
    // Face indices: 0 = -Z, 1 = +Z, 2 = -X, 3 = +X, 4 = -Y, 5 = +Y
    /*for (int face = 0; face < 6; ++face) {
        glm::ivec3 offset = Chunk::neighborOffsets[face];
        glm::ivec3 neighborPos = key + offset;

        auto it = m_Chunks.find(neighborPos);
        if (it != m_Chunks.end()) {
            markChunkFaceDirty(neighborPos, face ^ 1); // mark neighbor’s face that borders this chunk
        }
    }*/

}

void World::unloadOutdatedChunks(const glm::ivec3& playerChunkPos) {
    // Unload out of view chunks on an unloading interval
    if (m_UnloadTimer >= UNLOAD_INTERVAL) {
        m_UnloadTimer = 0.0f;

        // Remove generated chunks outside view
        for (auto it = m_Chunks.begin(); it != m_Chunks.end();) {
            if (!isChunkInView(playerChunkPos, it->first)) {
                it = m_Chunks.erase(it);
            } else {
                it++;
            }
        }

        // Remove old air chunks as well
        m_AirChunks.erase(std::remove_if(m_AirChunks.begin(), m_AirChunks.end(),
                    [&](const glm::ivec3& pos) {
                    return !isChunkInView(playerChunkPos, pos);
                    }), m_AirChunks.end());
    }
}

void World::queueChunkForRemeshing(const glm::ivec3& pos) {
    if (m_MeshQueuedChunks.insert(pos).second) {
        m_MeshQueue.push(pos);
    }
}

glm::ivec3 World::worldToChunkCoords(const glm::vec3& position) const {
    return glm::floor(position / glm::vec3(
                static_cast<float>(Chunk::kChunkWidth),
                static_cast<float>(Chunk::kChunkHeight),
                static_cast<float>(Chunk::kChunkDepth)
                ));
}

void World::markChunkFaceDirty(const glm::ivec3& chunkCoord, int faceIndex) {
    assert(faceIndex <= 6 && faceIndex >=0);
    auto it = m_Chunks.find(chunkCoord);
    if (it == m_Chunks.end()) return;

    it->second->markFaceDirty(faceIndex);

    if (m_MeshQueuedChunks.insert(chunkCoord).second) {
        m_MeshQueue.push(chunkCoord);
    }
}

void World::draw() {
    for (const auto& [coord, chunk] : m_Chunks) {
        chunk->draw();
    }
}

Player* World::getPlayer() {
    return &m_Player;
}

BlockType World::getBlockAtWorld(const glm::ivec3& pos) const {
    glm::ivec3 chunkCoords = worldToChunkCoords(glm::vec3(pos.x, pos.y, pos.z));
    auto it = m_Chunks.find(chunkCoords);
    if (it == m_Chunks.end()) return BlockType::Air;

    glm::ivec3 localCoords = {
        pos.x - chunkCoords.x * Chunk::kChunkWidth,
        pos.y - chunkCoords.y * Chunk::kChunkHeight,
        pos.z - chunkCoords.z * Chunk::kChunkDepth
    };

    return it->second->getBlock(localCoords.x, localCoords.y, localCoords.z);
}
Chunk* World::getChunkAtWorld(const glm::ivec3& pos) const {
    glm::ivec3 chunkCoords = worldToChunkCoords(glm::vec3(pos.x, pos.y, pos.z));
    auto it = m_Chunks.find(chunkCoords);
    if (it == m_Chunks.end()) return nullptr;

    return it->second.get();
}

Chunk* World::getChunkAtChunkPos(const glm::ivec3& chunkCoords) const {
    auto it = m_Chunks.find(chunkCoords);
    if (it == m_Chunks.end()) return nullptr;

    return it->second.get();
}

bool World::isChunkInView(const glm::ivec3& playerChunk, const glm::ivec3& chunkCoord) const {
    glm::ivec3 d = glm::abs(chunkCoord - playerChunk);
    return d.x <= VIEW_DISTANCE && d.y <= VIEW_DISTANCE && d.z <= VIEW_DISTANCE;
}
