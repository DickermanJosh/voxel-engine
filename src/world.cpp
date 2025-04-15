#include "World.hpp"
#include "ChunkGenerator.hpp"
#include <iostream>
#include <unordered_set>


World::World(uint64_t seed) 
    : m_Seed(seed), m_ChunkGenerator(seed) {
    std::cout << "World init with seed: " << seed << std::endl;
    getChunk(0, 0, 0);
};

void World::update(Camera& camera, float dt) {
    /*glm::vec3 pos = camera.getPosition();

    glm::ivec3 playerChunk = glm::floor(pos / glm::vec3(Chunk::kChunkWidth,
                Chunk::kChunkHeight,
                Chunk::kChunkDepth));

    std::unordered_set<glm::ivec3> requiredChunks;

    for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
        for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; ++dy) {
            for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
                glm::ivec3 offset(dx, dy, dz);
                glm::ivec3 chunkCoord = playerChunk + offset;

                requiredChunks.insert(chunkCoord);

                // Load chunk if needed
                getChunk(chunkCoord.x, chunkCoord.y, chunkCoord.z);
            }
        }
    }

    return;
    // Unload chunks outside of view distance
    for (auto it = m_Chunks.begin(); it != m_Chunks.end(); ) {
        if (requiredChunks.find(it->first) == requiredChunks.end()) {
            it = m_Chunks.erase(it); // Erase and move to next
        } else {
            ++it;
        }
    }*/
}

Chunk* World::getChunk(int cx, int cy, int cz) {
    glm::ivec3 key(cx, cy, cz);

    auto it = m_Chunks.find(key);
    if (it != m_Chunks.end()) {
        // Already exists
        return it->second.get();
    }

    // Create & generate a new Chunk
    std::unique_ptr<Chunk> newChunk = std::make_unique<Chunk>(
        glm::vec3(cx * Chunk::kChunkWidth,
                  cy * Chunk::kChunkHeight,
                  cz * Chunk::kChunkDepth));

    // Generate chunk blocks based on noise:
    // TODO: create a new one with m_Seed in the header. For :
    m_ChunkGenerator.populateChunk(*newChunk, cx, cy, cz);

    // Store in the map
    Chunk* chunkPtr = newChunk.get();
    m_Chunks[key] = std::move(newChunk);
    return chunkPtr;
}

void World::draw() {
    for (const auto& [coord, chunk] : m_Chunks) {
        chunk->draw();
    }
}
