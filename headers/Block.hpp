// Block.hpp
#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Mesh.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <utility>

class Block {
public:
    // Order of faces we assume:
    //  0 -> Negative Z
    //  1 -> Positive Z
    //  2 -> Negative X
    //  3 -> Positive X
    //  4 -> Negative Y
    //  5 -> Positive Y
    Block(const glm::vec3& position, const std::vector<bool>& visibleFaces);
    virtual ~Block() = default;

    void draw() const;

protected:
    // Helper meant to be overridden by inheriting classes 
    // to determine which tile each face will use
    virtual std::pair<int, int> getFaceTile(int faceIndex) const;

    // Actually builds the mesh from base face geometry, adjusting UVs
    void generateMesh(const std::vector<bool>& visibleFaces);
    
    glm::vec3 position;
    Mesh mesh;
};

#endif // BLOCK_HPP
