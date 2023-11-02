#pragma once
#include <vector>
#include <optional>

#include "node.hpp"
#include "armature.hpp"

namespace AssetLib::Structure
{
    //Contains all information used during loading
    class ModelContext
    {
        public:
        std::vector<Node*> nodes;
        bool isSkinned;
        std::optional<Armature> armature;
    };
};