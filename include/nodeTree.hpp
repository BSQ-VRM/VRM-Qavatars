#pragma once

#include <vector>
#include "UnityEngine/GameObject.hpp"

namespace VRM
{
    class Node
    {
    public:
        UnityEngine::GameObject* object;
    };

    class NodeTree
    {
    public:
        std::vector<Node> nodes;
    };
};