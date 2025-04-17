#include "World.hpp"
#include "ChunkGenerator.hpp"
#include <iostream>

World::World(uint64_t seed) 
    : m_Seed(seed), 
    m_ChunkGenerator(this, seed),
    m_Player(glm::vec3(0.0f, 150.0f, 0.0f)) {
    std::cout << "World init with seed: " << seed << std::endl;
    m_LastKnownPlayerChunk = worldToChunkCoords(m_Player.getPosition());
    };

void World::update(float dt) {
    m_Player.update(dt);

    glm::ivec3 playerChunk = worldToChunkCoords(m_Player.getPosition());
    bool playerChangedChunks = playerChunk != m_LastKnownPlayerChunk;

    if (playerChangedChunks) {
        m_LastKnownPlayerChunk = playerChunk;
        std::vector<glm::ivec3> candidates;
        std::unordered_set<glm::ivec3> visibleNow;

        for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
            for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; ++dy) {
                for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
                    glm::ivec3 pos = playerChunk + glm::ivec3(dx, dy, dz);
                    visibleNow.insert(pos);

                    // Ready all into a candidates list for middle-out loading
                    if (m_Chunks.find(pos) == m_Chunks.end() &&
                            m_QueuedChunks.find(pos) == m_QueuedChunks.end()) {
                        candidates.push_back(pos);
                    }
                }
            }
        }

        auto manhattanDistSq = [](const glm::ivec3& a, const glm::ivec3& b) {
            glm::ivec3 d = a - b;
            return d.x * d.x + d.y * d.y + d.z * d.z;
        };

        // Sort the visible chunks by their distance to the player. Closer chunks first
        std::sort(candidates.begin(), candidates.end(), [&](const glm::ivec3& a, const glm::ivec3& b) {
                return manhattanDistSq(a, playerChunk) < manhattanDistSq(b, playerChunk);
                });

        // Enqueue by distance
        for (const auto& pos : candidates) {
            m_ChunkGenQueue.push(pos);
            m_QueuedChunks.insert(pos);
        }

        // Remove queued chunks outside view
        std::queue<glm::ivec3> newQueue;
        for (; !m_ChunkGenQueue.empty(); m_ChunkGenQueue.pop()) {
            glm::ivec3 pos = m_ChunkGenQueue.front();
            if (visibleNow.find(pos) != visibleNow.end()) {
                newQueue.push(pos); // still valid
            } else {
                m_QueuedChunks.erase(pos);
            }
        }
        m_ChunkGenQueue = std::move(newQueue);

        // Remove generated chunks outside view
        for (auto it = m_Chunks.begin(); it != m_Chunks.end();) {
            if (!isChunkInView(playerChunk, it->first)) {
                it = m_Chunks.erase(it); // auto-deletes chunk due to unique_ptr
            } else {
                ++it;
            }
        }

        // Optionally remove air chunks as well (if you cache lots)
        m_AirChunks.erase(std::remove_if(m_AirChunks.begin(), m_AirChunks.end(),
                    [&](const glm::ivec3& pos) {
                    return !isChunkInView(playerChunk, pos);
                    }), m_AirChunks.end());
    }

    // Process the chunk gen queue (limit per frame until thread pool is available)
    for (int i = 0; i < 5 && !m_ChunkGenQueue.empty(); i++) {
        glm::ivec3 coords = m_ChunkGenQueue.front();
        m_ChunkGenQueue.pop();
        m_QueuedChunks.erase(coords);

        // TODO: DEFER THIS RENDERING TO A RENDER QUEUE!!
        Chunk* c = getChunk(coords.x, coords.y, coords.z);
        if (c) c->generateMesh();
    }
}

Chunk* World::getChunk(int cx, int cy, int cz) {
    glm::ivec3 key(cx, cy, cz);

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

    // Create and generate a new chunk
    auto newChunk = m_ChunkGenerator.generateChunk(cx, cy, cz);

    // If the chunk only contains air mark it for future reference
    if (newChunk == nullptr) {
        m_AirChunks.push_back(key);
        return nullptr;
    }

    // Store valid chunks in the chunk map
    Chunk* chunkPtr = newChunk.get();
    m_Chunks[key] = std::move(newChunk);
    // std::cout << "Chunks map size: "<< m_Chunks.size() << std::endl;
    return chunkPtr;
}

glm::ivec3 World::worldToChunkCoords(const glm::vec3& position) const {
    return glm::floor(position / glm::vec3(
                static_cast<float>(Chunk::kChunkWidth),
                static_cast<float>(Chunk::kChunkHeight),
                static_cast<float>(Chunk::kChunkDepth)
                ));
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

bool World::isChunkInView(const glm::ivec3& playerChunk, const glm::ivec3& chunkCoord) const {
    glm::ivec3 d = glm::abs(chunkCoord - playerChunk);
    return d.x <= VIEW_DISTANCE && d.y <= VIEW_DISTANCE && d.z <= VIEW_DISTANCE;
}
