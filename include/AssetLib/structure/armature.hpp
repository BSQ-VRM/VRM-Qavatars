#pragma once
#include <vector>

#include "node.hpp"

namespace AssetLib::Structure
{
    struct Armature
    {
        std::vector<Node*> bones;
        Node* rootBone;
    };
};