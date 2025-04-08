#include "Block.hpp"

Block::Block(const glm::vec3& position, const std::vector<bool>& visibleFaces)
    : position(position), mesh({}, {}) {
        generateMesh(visibleFaces);
    }

void Block::generateMesh(const std::vector<bool>& visibleFaces) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Face definitions (positions)
    const std::vector<float> faceVertices[6] = {
        // Negative Z face
        {
            // x     y      z    texcoords
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

    // Face definitions (indices)
    const std::vector<unsigned int> faceIndices = {
        0, 1, 2, 2, 3, 0
    };

    // Add visible faces to vertices and indices
    for (int i = 0; i < 6; ++i) {
        if (visibleFaces[i]) {
            int indexOffset = vertices.size() / 5; // 3 elements per vertex (3 positions, 2 texture coords)
            vertices.insert(vertices.end(), faceVertices[i].begin(), faceVertices[i].end());
            for (unsigned int idx : faceIndices) {
                indices.push_back(indexOffset + idx);
            }
        }
    }

    // Create mesh with the generated vertices and indices
    // TODO: Make this mesh for each chunk, NOT each block.
    mesh = Mesh(vertices, indices);
    mesh.setupMesh();
}

void Block::draw() const {
    mesh.draw();
}

