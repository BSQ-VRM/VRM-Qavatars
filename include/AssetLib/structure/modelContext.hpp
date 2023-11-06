#pragma once
#include <vector>
#include <optional>

#include "node.hpp"
#include "armature.hpp"
#include "assimp/scene.h"

namespace AssetLib::Structure
{
    //Contains all information used during loading
    class ModelContext
    {
        public:
        const aiScene* originalScene;

        std::vector<Node*> nodes;
        Node* rootNode;

        bool isSkinned;
        std::optional<Armature> armature;

        ModelContext() = default;
        virtual ~ModelContext() {}
    };
};