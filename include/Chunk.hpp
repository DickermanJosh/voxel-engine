#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Mesh.hpp"
#include "Block.hpp"
#include "SparseChunkData.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <optional>
#include <memory>

class World;

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

        enum class StorageMode {
            Dense,
            Sparse
        };
    public:
        void setBlock(int x, int y, int z, BlockType type);
        BlockType getBlock(int x, int y, int z) const;
        void generateMesh();
        void generateDirtyMesh();
        void remeshFaceTowardsNeighbor(int faceIndex); 
        void markFaceDirty(int faceIndex);
        void markAllFacesDirty();
        void draw() const;

        Chunk(World* world, const glm::vec3& position, StorageMode mode = StorageMode::Dense);

    private:
        glm::vec3 m_Position;
        std::vector<BlockType> m_Blocks;
        std::vector<std::optional<Block>> m_BlockObjs;
        std::unique_ptr<SparseChunkData> m_Sparse;
        Mesh m_Mesh;
        std::array<MeshPack, 6> m_FaceMeshPacks;
        StorageMode m_Mode;
        World* m_World;
        bool m_OnlyAir = true;
        uint8_t m_DirtyFaces = 0; // 6-bit mask: 1 = dirty, 0 = clean
    private:
        std::optional<Block> getBlockObj(int x, int y, int z) const;
        void determineVisibleFacesInChunk();
        bool isBlockActive(int x, int y, int z) const; // Helper that returns whether a block at (x,y,z) is a rendered type or air
        bool hasDirtyFaces() const;

        inline int index(int x, int y, int z) const {  // Helper to index into the blocks array.
            return x + kChunkWidth * (z + kChunkDepth * y); // Flatten 3D index into 1D
        }
};

#endif
