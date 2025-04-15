#include "Chunk.hpp"
#include <iostream>

Chunk::Chunk(const glm::vec3& pos)
    : m_Position(pos),
      m_Blocks((kChunkWidth * kChunkHeight * kChunkDepth), BlockType::Air), // default everything to BlockType::Air
      m_BlockObjs((kChunkWidth * kChunkHeight * kChunkDepth)), // default everything with std::optional
      m_Mesh({}) {

          std::cout << "Creating chunk @ pos: " << pos.x << ", " << pos.y << "," << pos.z << std::endl;
}

void Chunk::setBlock(int x, int y, int z, BlockType type) {
    m_Blocks[index(x,y,z)] = type;

    if (type == BlockType::Air) {
        m_BlockObjs[index(x,y,z)] = std::nullopt;
        return;
    }

    glm::vec3 worldSpaceCoords = {x + m_Position.x, y + m_Position.y, z + m_Position.z};
    m_BlockObjs[index(x,y,z)] = Block(type, worldSpaceCoords);
}

void Chunk::generateMesh() {
    MeshPack pack;

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

                    // TODO: Cull faces in between chunks
                    // Ask world for chunk in dir we are facing
                    // if it exists translate the pos to find the neighboring block and see its state
                    // if not, ask the world to generate it if it is inside out render distance
                    // if not in render distance set face to visible
                    if (!neighborInRange) {
                        visibleFaces.push_back(true);
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

    std::cout << "Vertices: " << pack.vertices.size() << std::endl;
    std::cout << "Indices: " << pack.indices.size() << std::endl;
}

void Chunk::draw() const {
    m_Mesh.draw();
}

BlockType Chunk::getBlock(int x, int y, int z) const {
    return m_Blocks[index(x, y, z)];
}

std::optional<Block> Chunk::getBlockObj(int x, int y, int z) const {
    return m_BlockObjs[index(x, y, z)];
}
