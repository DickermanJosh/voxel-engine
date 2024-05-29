// Block.hpp
#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Mesh.hpp"
#include <glm/glm.hpp>

class Block {
public:
    // TODO: add blocktype 
    Block(const glm::vec3& position);
    void draw() const;

private:
    glm::vec3 position;
    Mesh mesh;
};

#endif // BLOCK_HPP
