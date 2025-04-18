#include "Chunk.hpp"
#include "World.hpp"

Chunk::Chunk(World* world, const glm::vec3& pos, StorageMode mode)
    : m_World(world),
    m_Position(pos),
    m_Blocks((kChunkWidth * kChunkHeight * kChunkDepth), BlockType::Air), // default everything to BlockType::Air
    m_BlockObjs((kChunkWidth * kChunkHeight * kChunkDepth)), // default everything with std::optional
    m_Mesh({}), 
    m_Mode(mode) {
        // std::cout << "Creating chunk @ pos: " << pos.x << ", " << pos.y << "," << pos.z << std::endl;
        if (m_Mode == StorageMode::Dense) {
            m_Blocks.resize(kChunkDepth * kChunkDepth * kChunkHeight, BlockType::Air);
        } else {
            m_Sparse = std::make_unique<SparseChunkData>();
        }
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

    MeshPack pack;

    // todo change to m_blocks.size() for test
    pack.vertices.reserve(kChunkWidth * kChunkHeight * kChunkDepth * 6 * 4 * 5); // Rough upper bound
    pack.indices.reserve(kChunkWidth * kChunkHeight * kChunkDepth * 6 * 6);

    //fetch face indices once from Block:
    const std::vector<unsigned int>& faceIndices = Block::getFaceIndices();

    // For each block in this chunk:
    for (int y = 0; y < kChunkHeight; ++y) {
        for (int z = 0; z < kChunkDepth; ++z) {
            for (int x = 0; x < kChunkWidth; ++x) {
                BlockType blockType = getBlock(x,y,z);
                if (blockType == BlockType::Air) {
                    // Skip empty
                    continue;
                }

                std::vector<bool> visibleFaces;

                // For each face:
                for (int faceIndex = 0; faceIndex < 6; ++faceIndex) {
                    // Find neighbor coords
                    int nx = x + neighborOffsets[faceIndex].x;
                    int ny = y + neighborOffsets[faceIndex].y;
                    int nz = z + neighborOffsets[faceIndex].z;

                    // Determine if neighbor is in bounds:
                    bool neighborInRange = (nx >= 0 && nx < kChunkWidth &&
                            ny >= 0 && ny < kChunkHeight &&
                            nz >= 0 && nz < kChunkDepth);
                    
                    if (!neighborInRange) {
                        glm::ivec3 worldPos(
                                static_cast<int>(m_Position.x) + x + neighborOffsets[faceIndex].x,
                                static_cast<int>(m_Position.y) + y + neighborOffsets[faceIndex].y,
                                static_cast<int>(m_Position.z) + z + neighborOffsets[faceIndex].z
                                );

                        BlockType neighbor = m_World->getBlockAtWorld(worldPos);

                        if (neighbor != BlockType::Air) {
                            visibleFaces.push_back(false);
                        } else {
                            visibleFaces.push_back(true);
                        }

                        continue;
                    }

                    // If neighbor is in range but type is BlockType::Air => face is visible
                    // Otherwise, face is hidden
                    if (getBlock(nx,ny,nz) != BlockType::Air) {
                        visibleFaces.push_back(false);
                        continue;
                    }

                    visibleFaces.push_back(true);
                }

                std::optional<Block> b = getBlockObj(x, y, z);
                b->defineRenderedFaces(pack, visibleFaces);
            }
        }
    }

    // Upload to the GPU as one big mesh
    m_Mesh = Mesh(pack);
    m_Mesh.setupMesh();

    // std::cout << "Vertices: " << pack.vertices.size() << std::endl;
    // std::cout << "Indices: " << pack.indices.size() << std::endl;
}

void Chunk::remeshFaceTowardsNeighbor(int faceIndex) {
    // TODO: Use a dirty face mask to only regenerate dirty faces
    generateMesh();
}

void Chunk::draw() const {
    m_Mesh.draw();
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


