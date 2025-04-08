#ifndef STONEBLOCK_HPP
#define STONEBLOCK_HPP

#include "Block.hpp"

class StoneBlock : public Block {
public:
    StoneBlock(const glm::vec3& position, const std::vector<bool>& visibleFaces);
    virtual ~StoneBlock() = default;
protected:
    std::pair<int, int> getFaceTile(int faceIndex) const override;
};

#endif

