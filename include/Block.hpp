// Block.hpp
#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <glm/glm.hpp>
#include <vector>
#include <MeshPack.hpp>
#include <BlockType.hpp>
#include <BlockTextureMap.hpp>

// 16 x 16 tile texture atlas
static constexpr float TILE_SIZE = 1.0f / 16.0f;

// Face definition indices
static const std::vector<unsigned int> baseFaceIndices = {
    0, 1, 2, // triangle 1 
    2, 3, 0  // triangle 2
};

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

class Block {
    public:
        BlockType type;
    public:
        static std::vector<unsigned int> getFaceIndices();
        static std::vector<float> getFaceVertices();
        // Order of faces we assume:
        //  0 -> Negative Z
        //  1 -> Positive Z
        //  2 -> Negative X
        //  3 -> Positive X
        //  4 -> Negative Y
        //  5 -> Positive Y
        void defineRenderedFaces(MeshPack& pack, const std::vector<bool>& visibleFaces);

        Block(BlockType type, const glm::vec3& position);
        virtual ~Block() = default;

    protected:
        glm::vec3 m_Position;
        // std::vector<float> m_FaceData;
};

#endif // BLOCK_HPP
