#include "World.hpp"
#include "ChunkGenerator.hpp"
#include <iostream>

World::World(uint64_t seed) 
    : m_Seed(seed), 
    m_ChunkGenerator(this, seed),
    m_Player(glm::vec3(0.0f, 116.0f, 0.0f)) {
    std::cout << "World init with seed: " << seed << std::endl;

    glm::ivec3 pChunkPos = worldToChunkCoords(m_Player.getPosition());

    for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
        for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; ++dy) {
            for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
                getChunk(pChunkPos.x + dx, pChunkPos.y + dy, pChunkPos.z + dz);
            }
        }
    }

    for (auto& [coords, chunk] : m_Chunks) {
        chunk->generateMesh();
    }
};

void World::update(float dt) {
    m_Player.update(dt);
}

Chunk* World::getChunk(int cx, int cy, int cz) {
    glm::ivec3 key(cx, cy, cz);

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
    if (newChunk == nullptr) return nullptr;

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
