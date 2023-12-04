#pragma once
#include <string>
#include <vector>
#include <optional>

#include "interMeshData.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/GameObject.hpp"

#include "assimp/scene.h"

namespace AssetLib::Structure
{
    class Node
    {
        public:
        Node(std::string name, std::vector<Node*> children, Node* parent, bool isBone, bool isRoot, UnityEngine::Vector3 position, std::optional<InterMeshData> mesh, aiNode* originalNode)
        {
            this->name = name;
            this->children = children;
            this->parent = parent;
            this->isBone = isBone;
            this->isRoot = isRoot;
            this->position = position;
            this->mesh = mesh;
            this->originalNode = originalNode;
        }

        Node() = default;
        
        //Name of this node
        std::string name;

        //Child nodes
        std::vector<Node*> children;
        //Parent node
        Node* parent = nullptr;

        //Whether the node is a bone
        bool isBone;
        //Whether the node is the root node
        bool isRoot;

        //Whether or not an gameobject has been associated with this node
        bool processed;
        //Gameobject associated with this node
        UnityEngine::GameObject* gameObject = nullptr;

        //LOCAL TO PARENT NODE
        Sombrero::FastVector3 position;

        std::optional<InterMeshData> mesh;

        aiNode* originalNode = nullptr;
    };
};