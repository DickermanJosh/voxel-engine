#include "World.hpp"
#include "ChunkGenerator.hpp"
#include <iostream>

World::World(uint64_t seed) 
    : m_Seed(seed), 
    m_ChunkGenerator(this, seed),
    m_Player(glm::vec3(0.0f, 150.0f, 0.0f)) {
        std::cout << "World init with seed: " << seed << std::endl;
        m_LastKnownPlayerChunk = worldToChunkCoords(m_Player.getPosition());
        enqueueNearbyChunks(m_LastKnownPlayerChunk);
    };


int maxPerFrame = 2;
void World::update(float dt) {
    m_UnloadTimer += dt;
    m_Player.update(dt);

    glm::ivec3 playerChunk = worldToChunkCoords(m_Player.getPosition());
    bool playerChangedChunks = playerChunk != m_LastKnownPlayerChunk;

    unloadOutdatedChunks(playerChunk);

    // When the player crosses a chunk border we want to update our list of viable chunks to generate and render
    // This means searching through the render distance limit to see which chunks are not yet loaded or queued
    // Newly added chunks are sorted by their distance to the player
    if (playerChangedChunks) {
        m_LastKnownPlayerChunk = playerChunk;
        enqueueNearbyChunks(playerChunk);
    }

    // Process the chunk gen queue (limit per frame until thread pool is available)
    for (int i = 0; i < maxPerFrame && !m_ChunkGenQueue.empty(); i++) {
        glm::ivec3 coords = m_ChunkGenQueue.front();
        m_ChunkGenQueue.pop();
        m_QueuedChunks.erase(coords);

        Chunk* c = getChunk(coords.x, coords.y, coords.z);
        // if (c) c->generateMesh();
        if (c && m_MeshQueuedChunks.insert(coords).second) {
            m_MeshQueue.push(coords);
            // c->markAllFacesDirty();
            for (int face = 0; face < 6; face++) {
                glm::ivec3 neighborPos = coords + Chunk::neighborOffsets[face];
                auto it = m_Chunks.find(neighborPos);
                if (it != m_Chunks.end()) {
                    int flipped = face ^ 1;
                    assert(Chunk::neighborOffsets[face] + Chunk::neighborOffsets[flipped] == glm::ivec3(0));
                    markChunkFaceDirty(neighborPos, flipped); // flip face to point toward this chunk
                }
            }
        }
    }

    // Generate meshes at the front of the mesh queue
    for (int i = 0; i < maxPerFrame && !m_MeshQueue.empty(); ++i) {
        glm::ivec3 pos = m_MeshQueue.front();
        m_MeshQueue.pop();
        m_MeshQueuedChunks.erase(pos);

        auto it = m_Chunks.find(pos);
        if (it != m_Chunks.end()) {
            it->second->generateMesh();
            // Request all neighbords to generate a new dirty mesh
            for (int x = -1; x < 1; x++) {
                for (int y = -1; y < 1; y++) {
                    for (int z = -1; z < 1; z++) {
                        glm::ivec3 nPos(pos.x + x, pos.y + y, pos.z + z);
                        auto nIt = m_Chunks.find(nPos);
                        if (nIt != m_Chunks.end()) {
                            nIt->second->generateDirtyMesh();
                        }
                    }
                }
            }
        }
    }

    std::cout<< "Loaded Chunks: " << m_Chunks.size() << std::endl;
}

void World::enqueueNearbyChunks(const glm::ivec3& playerChunkPos) {
    std::vector<glm::ivec3> candidates;
    std::unordered_set<glm::ivec3> visibleNow;

    for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
        for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; ++dy) {
            for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
                glm::ivec3 pos = playerChunkPos + glm::ivec3(dx, dy, dz);
                visibleNow.insert(pos);
                assert(std::abs(dx) <= VIEW_DISTANCE);
                assert(std::abs(dy) <= VIEW_DISTANCE);
                assert(std::abs(dz) <= VIEW_DISTANCE);
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
            return manhattanDistSq(a, playerChunkPos) < manhattanDistSq(b, playerChunkPos);
            });

    // Enqueue by distance
    for (const auto& pos : candidates) {
        assert(m_Chunks.find(pos) == m_Chunks.end());
        assert(m_QueuedChunks.find(pos) == m_QueuedChunks.end());
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
            m_QueuedChunks.erase(pos); // not valid
        }
    }

    m_ChunkGenQueue = std::move(newQueue);

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

    if (m_ChunkGenerator.isChunkEmpty(cx, cy, cz)) {
        m_AirChunks.push_back(key);
        return nullptr;
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

    // Ask all neighboring chunks to re-check their meshing on the face
    // facing toward this chunk
    // Face indices: 0 = -Z, 1 = +Z, 2 = -X, 3 = +X, 4 = -Y, 5 = +Y
    for (int face = 0; face < 6; ++face) {
        glm::ivec3 offset = Chunk::neighborOffsets[face];
        glm::ivec3 neighborPos = key + offset;

        auto it = m_Chunks.find(neighborPos);
        if (it != m_Chunks.end()) {
            markChunkFaceDirty(neighborPos, face ^ 1); // mark neighbor’s face that borders this chunk
        }
    }

    return chunkPtr;
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

bool World::isChunkInView(const glm::ivec3& playerChunk, const glm::ivec3& chunkCoord) const {
    glm::ivec3 d = glm::abs(chunkCoord - playerChunk);
    return d.x <= VIEW_DISTANCE && d.y <= VIEW_DISTANCE && d.z <= VIEW_DISTANCE;
}
