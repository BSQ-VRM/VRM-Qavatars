#pragma once
#include <vector>
#include <optional>

#include "node.hpp"
#include "armature.hpp"
#include "assimp/shared/assimp/scene.h"
#include "UnityEngine/GameObject.hpp"

namespace AssetLib::Structure
{
    //Contains all information used during loading
    class ModelContext
    {
        public:
        std::string fileName;
        const aiScene* originalScene;

        std::vector<Node*> nodes;
        Node* rootNode;

        bool isSkinned;
        std::optional<Armature> armature;

        UnityEngine::GameObject* rootGameObject;

        ModelContext() = default;
        virtual ~ModelContext() {}
        ModelContext(ModelContext&&) = default;
    };
};