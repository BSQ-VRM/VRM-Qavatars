#pragma once
#include <string>
#include <vector>
#include <optional>

#include "interMeshData.hpp"
#include "UnityEngine/Vector3.hpp"

namespace AssetLib::Structure
{
    class Node
    {
        public:
        std::string name;

        std::vector<Node*> children;
        Node* parent;

        bool isBone;
        bool isRoot;

        //LOCAL TO PARENT NODE
        UnityEngine::Vector3 position;

        std::optional<InterMeshData> mesh;
    };
};