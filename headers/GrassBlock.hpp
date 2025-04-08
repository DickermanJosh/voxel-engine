#ifndef GRASSBLOCK_HPP
#define GRASSBLOCK_HPP

#include "Block.hpp"

class GrassBlock : public Block {
public:
    GrassBlock(const glm::vec3& position, const std::vector<bool>& visibleFaces);
    virtual ~GrassBlock() = default;
protected:
    std::pair<int, int> getFaceTile(int faceIndex) const override;
};

#endif // GRASSBLOCK_HPP

