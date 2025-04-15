#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <glm/glm.hpp>
#include <vector>
#include <optional>
#include "Mesh.hpp"
#include "Block.hpp"

class Chunk {
    public:
        inline static constexpr int kChunkWidth = 16;
        inline static constexpr int kChunkHeight = 16;
        inline static constexpr int kChunkDepth = 16;

        // Offsets for neighbor cells in the order of faces:
        inline static constexpr glm::ivec3 neighborOffsets[6] = {
            { 0,  0, -1}, // -z
            { 0,  0,  1}, // +z
            {-1,  0,  0}, // -x
            { 1,  0,  0}, // +x
            { 0, -1,  0}, // -y
            { 0,  1,  0}  // +y
        };
    public:
        void setBlock(int x, int y, int z, BlockType type);
        void generateMesh();
        void draw() const;

        Chunk(const glm::vec3& position);
        virtual ~Chunk() = default;

    private:
        glm::vec3 m_Position;
        std::vector<BlockType> m_Blocks;
        std::vector<std::optional<Block>> m_BlockObjs;
        Mesh m_Mesh;
        MeshPack m_MeshPack;
    private:
        BlockType getBlock(int x, int y, int z) const;
        std::optional<Block> getBlockObj(int x, int y, int z) const;
        void determineVisibleFacesInChunk();
        bool isBlockActive(int x, int y, int z) const; // Helper that returns whether a block at (x,y,z) is a rendered type or air
        inline int index(int x, int y, int z) const {  // Helper to index into the blocks array.
            return x + kChunkWidth * (z + kChunkDepth * y); // Flatten 3D index into 1D
        }
};

#endif
