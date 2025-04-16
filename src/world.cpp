#include "World.hpp"
#include "ChunkGenerator.hpp"
#include <iostream>

World::World(uint64_t seed) 
    : m_Seed(seed), 
    m_ChunkGenerator(seed),
    m_Player(glm::vec3(0.0f, 16.0f, 0.0f)) {
    std::cout << "World init with seed: " << seed << std::endl;

    for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
        for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; ++dy) {
            for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
                Chunk* c = getChunk(dx, dy, dz);
                if (c != nullptr) c->generateMesh();
            }
        }
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

    // Determine if the chunk is in view range before generating it
    /*glm::ivec3 camPos = worldToChunkCoords(m_Player.getPosition());
    if (
            (camPos.x + VIEW_DISTANCE > cx || camPos.x - VIEW_DISTANCE < cx) ||
            (camPos.y + VIEW_DISTANCE > cy || camPos.y - VIEW_DISTANCE < cy) ||
            (camPos.z + VIEW_DISTANCE > cz || camPos.z - VIEW_DISTANCE < cz)
       ) {

        return nullptr;
    }*/

    // Create & generate a new Chunk
    std::unique_ptr<Chunk> newChunk = std::make_unique<Chunk>(
            this,
            glm::vec3(cx * Chunk::kChunkWidth,
                cy * Chunk::kChunkHeight,
                cz * Chunk::kChunkDepth));

    // Generate chunk blocks based on noise:
    // TODO: create a new one with m_Seed in the header. For :
    m_ChunkGenerator.populateChunk(*newChunk, cx, cy, cz);

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

BlockType World::getBlockAtWorld(int wx, int wy, int wz) const {
    glm::ivec3 chunkCoords = worldToChunkCoords(glm::vec3(wx, wy, wz));
    auto it = m_Chunks.find(chunkCoords);
    if (it == m_Chunks.end()) return BlockType::Air;

    glm::ivec3 localCoords = {
        wx - chunkCoords.x * Chunk::kChunkWidth,
        wy - chunkCoords.y * Chunk::kChunkHeight,
        wz - chunkCoords.z * Chunk::kChunkDepth
    };

    return it->second->getBlock(localCoords.x, localCoords.y, localCoords.z);
}
