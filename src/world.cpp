#include "World.hpp"
#include "ChunkGenerator.hpp"
#include <iostream>

World::World(uint64_t seed) 
    : m_Seed(seed), 
    m_ChunkGenerator(this, seed),
    m_Player(glm::vec3(0.0f, 150.0f, 0.0f)) {
    std::cout << "World init with seed: " << seed << std::endl;

    glm::ivec3 playerChunk = worldToChunkCoords(m_Player.getPosition());

    std::vector<glm::ivec3> candidates;

    for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
        for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; ++dy) {
            for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
                glm::ivec3 pos = playerChunk + glm::ivec3(dx, dy, dz);

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

    std::sort(candidates.begin(), candidates.end(), [&](const glm::ivec3& a, const glm::ivec3& b) {
        return manhattanDistSq(a, playerChunk) < manhattanDistSq(b, playerChunk);
    });

    for (const auto& pos : candidates) {
        m_ChunkGenQueue.push(pos);
        m_QueuedChunks.insert(pos);
    }

    /*for (auto& [coords, chunk] : m_Chunks) {
        chunk->generateMesh();
    }*/
};

void World::update(float dt) {
    m_Player.update(dt);

    // Process the chunk gen queue
    for (int i = 0; i < 5 && !m_ChunkGenQueue.empty(); i++) {
        glm::ivec3 coords = m_ChunkGenQueue.front();
        m_ChunkGenQueue.pop();
        m_QueuedChunks.erase(coords);

        // TODO: DEFER THIS RENDERING TO A RENDER QUEUE!!
        Chunk* c = getChunk(coords.x, coords.y, coords.z);
        // Add to render queue
        if (c) c->generateMesh();
    }
}

Chunk* World::getChunk(int cx, int cy, int cz) {
    glm::ivec3 key(cx, cy, cz);

    // first search known air chunks
    for (size_t i = 0; i < m_AirChunks.size(); i++) {
        if (key == m_AirChunks[i]) return nullptr;
    }

    auto it = m_Chunks.find(key);
    if (it != m_Chunks.end()) {
        // Already exists
        return it->second.get();
    }

    // Create & generate a new Chunk
    /*std::unique_ptr<Chunk> newChunk = std::make_unique<Chunk>(
            this,
            glm::vec3(cx * Chunk::kChunkWidth,
                cy * Chunk::kChunkHeight,
                cz * Chunk::kChunkDepth)); */

    // Generate chunk blocks based on noise:
    // m_ChunkGenerator.populateChunk(*newChunk, cx, cy, cz);
    auto newChunk = m_ChunkGenerator.generateChunk(cx, cy, cz);
    if (newChunk == nullptr) {
        m_AirChunks.push_back(key);
        return nullptr;
    }

    // Store in the map
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

