#include "Chunk.hpp"

Chunk::Chunk(const glm::vec3& pos)
    : m_Position(pos),
      m_Blocks((kChunkWidth * kChunkHeight * kChunkDepth), BlockType::Air), // default everything to BlockType::Air
      m_BlockObjs((kChunkWidth * kChunkHeight * kChunkDepth)), // default everything with std::optional
      m_Mesh({})
{
    // basic testing
    for (int y = 0; y < kChunkHeight; ++y) {
        for (int z = 0; z < kChunkDepth; ++z) {
            for (int x = 0; x < kChunkWidth; ++x) {
                if (y < kChunkHeight - 1) {
                    m_Blocks[index(x,y,z)] = BlockType::Stone;
                    m_BlockObjs[index(x,y,z)] = Block(BlockType::Stone, glm::vec3(x,y,z));
                } else if (y == kChunkHeight - 1) {
                    m_Blocks[index(x,y,z)] = BlockType::Grass;
                    m_BlockObjs[index(x,y,z)] = Block(BlockType::Grass, glm::vec3(x,y,z));
                } else {
                    m_Blocks[index(x,y,z)] = BlockType::Air;
                }
            }
        }
    }

    generateMesh();
}

void Chunk::generateMesh()
{
    m_MeshPack.vertices.reserve(kChunkWidth * kChunkHeight * kChunkDepth * 6 * 4 * 5); // Rough upper bound
    m_MeshPack.indices.reserve(kChunkWidth * kChunkHeight * kChunkDepth * 6 * 6);

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
                b->defineRenderedFaces(m_MeshPack, visibleFaces);
            }
        }
    }

    // Upload to the GPU as one big mesh
    m_Mesh = Mesh(m_MeshPack);
    m_Mesh.setupMesh();
}

void Chunk::draw() const
{
    m_Mesh.draw();
}

BlockType Chunk::getBlock(int x, int y, int z) const
{
    return m_Blocks[index(x, y, z)];
}

std::optional<Block> Chunk::getBlockObj(int x, int y, int z) const
{
    return m_BlockObjs[index(x, y, z)];
}
