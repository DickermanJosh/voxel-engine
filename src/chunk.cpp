#include "Chunk.hpp"
#include "World.hpp"
#include <iostream>

Chunk::Chunk(World* world, const glm::vec3& pos, StorageMode mode)
    : m_World(world),
    m_Position(pos),
    m_Blocks((kChunkWidth * kChunkHeight * kChunkDepth), BlockType::Air), // default everything to BlockType::Air
    m_BlockObjs((kChunkWidth * kChunkHeight * kChunkDepth)), // default everything with std::optional
    m_Mode(mode) {
        std::cout << "Creating chunk @ pos: " << pos.x << ", " << pos.y << "," << pos.z << std::endl;
        if (m_Mode == StorageMode::Dense) {
            m_Blocks.resize(kChunkDepth * kChunkDepth * kChunkHeight, BlockType::Air);
        } else {
            m_Sparse = std::make_unique<SparseChunkData>();
        }
    }

Chunk::~Chunk() {
    m_Blocks.clear();
    m_BlockObjs.clear();
    m_Sparse.reset();
    m_Mesh.reset();
}

void Chunk::setBlock(int x, int y, int z, BlockType type) {
    if (m_Mode == StorageMode::Dense) {
        m_Blocks[index(x,y,z)] = type;
    } else {
        m_Sparse->setBlock(x, y, z, type);
    }

    if (type == BlockType::Air) {
        m_BlockObjs[index(x,y,z)] = std::nullopt;
        return;
    }

    if (m_OnlyAir) m_OnlyAir = false;

    glm::vec3 worldSpaceCoords = {x + m_Position.x, y + m_Position.y, z + m_Position.z};
    m_BlockObjs[index(x,y,z)] = Block(type, worldSpaceCoords);
}

void Chunk::generateMesh() {
    if (m_OnlyAir) return;

    // Clear all face mesh packs
    /*for (auto& facePack : m_FaceMeshPacks) {
        facePack.vertices.clear();
        facePack.indices.clear();
    }*/

    m_Mesh.reset();

    MeshPack pack;

    pack.vertices.reserve(kChunkWidth * kChunkHeight * kChunkDepth * 6 * 4 * 5); // Rough upper bound
    pack.indices.reserve(kChunkWidth * kChunkHeight * kChunkDepth * 6 * 6);

    for (int y = 0; y < kChunkHeight; ++y) {
        for (int z = 0; z < kChunkDepth; ++z) {
            for (int x = 0; x < kChunkWidth; ++x) {
                BlockType blockType = getBlock(x, y, z);
                if (blockType == BlockType::Air) continue;

                std::optional<Block> b = getBlockObj(x, y, z);
                if (!b.has_value()) continue;

                std::vector<bool> visible(6, false);
                for (int face = 0; face < 6; ++face) {
                    int nx = x + neighborOffsets[face].x;
                    int ny = y + neighborOffsets[face].y;
                    int nz = z + neighborOffsets[face].z;

                    bool neighborInRange = (nx >= 0 && ny >= 0 && nz >= 0 &&
                                            nx < kChunkWidth && ny < kChunkHeight && nz < kChunkDepth);

                    bool faceVisible = false;

                    // Neighboring block belongs to another chunk
                    if (!neighborInRange) {
                        glm::ivec3 worldPos(
                                static_cast<int>(m_Position.x) + x + neighborOffsets[face].x,
                                static_cast<int>(m_Position.y) + y + neighborOffsets[face].y,
                                static_cast<int>(m_Position.z) + z + neighborOffsets[face].z
                                );

                        BlockType neighbor = m_World->getBlockAtWorld(worldPos);

                        if (neighbor == BlockType::Air) {
                            faceVisible = true;
                        }
                    } else {
                        faceVisible = (getBlock(nx, ny, nz) == BlockType::Air);
                    }
                    /*if (!neighborInRange) {
                        glm::ivec3 neighborChunkPos = glm::ivec3(m_Position) + neighborOffsets[face];
                        // glm::ivec3 chunkPos = glm::floor(m_Position / glm::vec3(kChunkWidth, kChunkHeight, kChunkDepth));
                        // glm::ivec3 neighborChunkPos = chunkPos + neighborOffsets[face];
                        Chunk* nChunk = m_World->getChunkAtChunkPos(neighborChunkPos);

                        if (nChunk == nullptr) {
                            faceVisible = true;
                        } else {
                            int localX = (x + neighborOffsets[face].x + kChunkWidth) % kChunkWidth;
                            if (localX < 0) localX += kChunkWidth;
                            int localY = (y + neighborOffsets[face].y + kChunkHeight) % kChunkHeight;
                            if (localY < 0) localX += kChunkHeight;
                            int localZ = (z + neighborOffsets[face].z + kChunkDepth) % kChunkDepth;
                            if (localZ < 0) localX += kChunkDepth;

                            faceVisible = nChunk->getBlock(localX, localY, localZ) == BlockType::Air;
                        }
                    } else {
                        faceVisible = getBlock(nx, ny, nz) == BlockType::Air;
                    }*/

                    visible[face] = faceVisible;
                    /*if (faceVisible) {
                        visible[face] = true;

                        int baseIndex = m_FaceMeshPacks[face].vertices.size() / 5;
                        b->defineRenderedFaces(m_FaceMeshPacks[face], visible);
                    }*/
                }
                b->defineRenderedFaces(pack, visible);
            }
        }
    }

    // Merge all face packs into the final mesh
    /*MeshPack combined;
    for (const auto& facePack : m_FaceMeshPacks) {
        int indexOffset = combined.vertices.size() / 5;
        combined.vertices.insert(combined.vertices.end(), facePack.vertices.begin(), facePack.vertices.end());

        for (unsigned int idx : facePack.indices) {
            combined.indices.push_back(idx + indexOffset);
        }
    }*/

    m_Mesh = std::make_unique<Mesh>(pack);
    m_Mesh->setupMesh();
    m_DirtyFaces = 0;
}

void Chunk::remeshFaceTowardsNeighbor(int faceIndex) {
    assert(faceIndex <= 6 && faceIndex >= 0);
    markFaceDirty(faceIndex);

    // glm::ivec3 chunkCoords = glm::floor(m_Position / glm::vec3(kChunkWidth, kChunkHeight, kChunkDepth));
    m_World->queueChunkForRemeshing(m_Position);
}

void Chunk::markFaceDirty(int faceIndex) {
    assert(faceIndex <= 6 && faceIndex >= 0);
    // m_DirtyFaces = 0b111111;
    m_DirtyFaces |= (1 << faceIndex);
}

void Chunk::markAllFacesDirty() {
    m_DirtyFaces = 0b111111;
    assert(m_DirtyFaces == 6);
}

bool Chunk::hasDirtyFaces() const {
    return m_DirtyFaces != 0;
}

// TODO figure out wtf is wrong withthis 
void Chunk::generateDirtyMesh() {
    m_DirtyFaces = 0;
    generateMesh();
    return;
for (int face = 0; face < 6; ++face) {
        // If the current face is not marked as dirty leave it alone
        if (!(m_DirtyFaces & (1 << face)))
            continue;

        // MeshPack& facePack = m_FaceMeshPacks[face];
        // facePack.vertices.clear();
        // facePack.indices.clear();

        int faceVertexCount = 0;

        for (int y = 0; y < kChunkHeight; ++y) {
            for (int z = 0; z < kChunkDepth; ++z) {
                for (int x = 0; x < kChunkWidth; ++x) {
                    BlockType block = getBlock(x, y, z);
                    if (block == BlockType::Air) continue;

                    int nx = x + neighborOffsets[face].x;
                    int ny = y + neighborOffsets[face].y;
                    int nz = z + neighborOffsets[face].z;

                    bool outOfBounds = (nx < 0 || ny < 0 || nz < 0 ||
                                        nx >= kChunkWidth || ny >= kChunkHeight || nz >= kChunkDepth);

                    if (outOfBounds || getBlock(nx, ny, nz) == BlockType::Air) {
                        std::optional<Block> b = getBlockObj(x, y, z);
                        if (!b.has_value()) continue;

                        std::vector<bool> faceFlags(6, false);
                        faceFlags[face] = true;

                        // int baseIndex = facePack.vertices.size() / 5;
                        // b->defineRenderedFaces(m_FaceMeshPacks, faceFlags);
                    }
                }
            }
        }
    }

    // Merge all face packs
    MeshPack final;
    /*for (const auto& facePack : m_FaceMeshPacks) {
        int offset = final.vertices.size() / 5;

        final.vertices.insert(final.vertices.end(), facePack.vertices.begin(), facePack.vertices.end());
        for (unsigned int idx : facePack.indices)
            final.indices.push_back(idx + offset);
    }*/

    m_Mesh = std::make_unique<Mesh>(final);
    m_Mesh->setupMesh();
    m_DirtyFaces = 0;
}

void Chunk::draw() const {
    if (!m_Mesh) return;
    m_Mesh->draw();
}

BlockType Chunk::getBlock(int x, int y, int z) const {
    assert(x >= 0 && x < kChunkWidth);
    assert(y >= 0 && y < kChunkHeight);
    assert(z >= 0 && z < kChunkDepth);

    if (m_Mode == StorageMode::Dense) {
        return m_Blocks[index(x, y, z)];
    }

    return m_Sparse->getBlock(x, y, z);
}

std::optional<Block> Chunk::getBlockObj(int x, int y, int z) const {
    assert(x >= 0 && x < kChunkWidth);
    assert(y >= 0 && y < kChunkHeight);
    assert(z >= 0 && z < kChunkDepth);
    return m_BlockObjs[index(x, y, z)];
}


