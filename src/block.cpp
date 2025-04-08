#include "Block.hpp"

// 16 x 16 tile texture atlas
static const float TILE_SIZE = 1.0f / 16.0f;

// For each face, we store 4 vertices. Each vertex has 5 floats:
//  x, y, z, u, v
// where (u,v) is in the 0..1 range for that face.
static const std::vector<float> baseFaceVertices[6] = {
    // Negative Z face
    {
        // x     y      z     u     v 
        -0.5f,  0.0f, -0.5f, 0.0f, 0.0f, // bottom-left
        0.5f,  0.0f, -0.5f, 1.0f, 0.0f, // bottom-right
        0.5f,  1.0f, -0.5f, 1.0f, 1.0f, // top-right
        -0.5f,  1.0f, -0.5f, 0.0f, 1.0f // top-left
    },

    // Positive Z face
    {
        -0.5f,  0.0f,  0.5f, 0.0f, 0.0f,
        -0.5f,  1.0f,  0.5f, 0.0f, 1.0f,
        0.5f,  1.0f,  0.5f, 1.0f, 1.0f,
        0.5f,  0.0f,  0.5f, 1.0f, 0.0f,
    },

    // Negative X face
    {
        -0.5f,  0.0f, -0.5f, 0.0f, 0.0f,
        -0.5f,  1.0f, -0.5f, 0.0f, 1.0f,
        -0.5f,  1.0f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.0f,  0.5f, 1.0f, 0.0f,
    },

    // Positive X face
    {
        0.5f,  0.0f, -0.5f, 0.0f, 0.0f,
        0.5,  0.0f,  0.5f, 1.0f, 0.0f,
        0.5f,  1.0f,  0.5f, 1.0f, 1.0f,
        0.5f,  1.0f, -0.5f, 0.0f, 1.0f,
    },

    // Negative Y face
    {
        -0.5f,  0.0f, -0.5f, 0.0f, 0.0f,
        -0.5f,  0.0f,  0.5f, 0.0f, 1.0f,
        0.5f,  0.0f,  0.5f, 1.0f, 1.0f,
        0.5f,  0.0f, -0.5f, 1.0f, 0.0f
    },

    // Positive Y face
    {
        -0.5f,  1.0f, -0.5f, 0.0f, 0.0f,
        0.5f,  1.0f, -0.5f, 1.0f, 0.0f,
        0.5f,  1.0f,  0.5f, 1.0f, 1.0f,
        -0.5f,  1.0f,  0.5f, 0.0f, 1.0f
    }
};

// Face definition indices
static const std::vector<unsigned int> baseFaceIndices = {
    0, 1, 2, // triangle 1 
    2, 3, 0  // triangle 2
};

///<summary>
/// remap (u,v) from [0..1, 0..1] into a sub-tile (tileX, tileY) in a 16x16 atlas
///<summary/>
static std::vector<float> remapFaceUV(const std::vector<float>& faceVerts,
                                      int tileX, int tileY)
{
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

Block::Block(const glm::vec3& position, const std::vector<bool>& visibleFaces)
    : position(position), mesh({}, {}) {
        // Intentionally empty,
        // mesh generation handled by children
    }


void Block::generateMesh(const std::vector<bool>& visibleFaces) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int face = 0; face < 6; face++) {
        if (!visibleFaces[face]) {
            continue; // skip hidden face
        }

        // Derived block can override face tile
        auto [tileX, tileY] = getFaceTile(face);

        // Copy base geometry & shift UVs
        auto faceData = remapFaceUV(baseFaceVertices[face], tileX, tileY);

        // Offset the face by the block's position
        for (size_t i = 0; i < faceData.size(); i += 5) {
            faceData[i + 0] += position.x; // X
            faceData[i + 1] += position.y; // Y
            faceData[i + 2] += position.z; // Z
        }

        // Insert into global arrays
        int idxOffset = vertices.size() / 5;
        vertices.insert(vertices.end(), faceData.begin(), faceData.end());

        for (auto idx : baseFaceIndices) {
            indices.push_back(idxOffset + idx);
        }
    }

    // Create mesh with the generated vertices and indices
    // TODO: Make this mesh for each chunk, NOT each block.
    mesh = Mesh(vertices, indices);
    mesh.setupMesh();
}

// Default: all faces use tile(0, 0). All derived blocks override this.
std::pair<int,int> Block::getFaceTile(int /*faceIndex*/) const {
    return {0, 0};
}

void Block::draw() const {
    mesh.draw();
}
