#include "Block.hpp"

///<summary>
/// remap (u,v) from [0..1, 0..1] into a sub-tile (tileX, tileY) in a 16x16 atlas
///<summary/>
static std::vector<float> remapFaceUV(const std::vector<float>& faceVerts,
                                      int tileX, int tileY) {
    std::vector<float> newFace = faceVerts; // copy
    for (size_t i = 0; i < newFace.size(); i += 5) {
        float oldU = newFace[i+3];
        float oldV = newFace[i+4];

        float newU = tileX * TILE_SIZE + oldU * TILE_SIZE;
        // float newV = tileY * TILE_SIZE + oldV * TILE_SIZE;
         float newV = (tileY + 1) * TILE_SIZE - oldV * TILE_SIZE;

        newFace[i+3] = newU;
        newFace[i+4] = newV;
    }
    return newFace;
}

Block::Block(BlockType type, const glm::vec3& position)
    : type(type), m_Position(position) {
    }


void Block::defineRenderedFaces(MeshPack& pack, const std::vector<bool>& visibleFaces) {
    for (int face = 0; face < 6; face++) {
        if (!visibleFaces[face]) {
            continue; // skip hidden face
        }

        // Derived block can override face tile
        auto [tileX, tileY] = getFaceTile(type, face);

        // Copy base geometry & shift UVs
        m_FaceData = remapFaceUV(baseFaceVertices[face], tileX, tileY);

        // Offset the face by the block's position
        for (size_t i = 0; i < m_FaceData.size(); i += 5) {
            m_FaceData[i + 0] += m_Position.x; // X
            m_FaceData[i + 1] += m_Position.y; // Y
            m_FaceData[i + 2] += m_Position.z; // Z
        }

        // Insert into global arrays
        int idxOffset = pack.vertices.size() / 5;
        pack.vertices.insert(pack.vertices.end(), m_FaceData.begin(), m_FaceData.end());

        for (auto idx : baseFaceIndices) {
            pack.indices.push_back(idxOffset + idx);
        }
    }
}

std::vector<unsigned int> Block::getFaceIndices() {
    return baseFaceIndices;
}
