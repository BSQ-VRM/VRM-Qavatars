#pragma once

#include "AssetLib/structure/modelContext.hpp"
#include "AssetLib/structure/node.hpp"
#include "UnityEngine/Mesh.hpp"

namespace AssetLib::Generators
{
    class MeshGenerator
    {
        public:
        MeshGenerator() = default;
        ~MeshGenerator() = default;

        void Generate(const AssetLib::Structure::Node* node, const AssetLib::Structure::ModelContext* context);
    };
};