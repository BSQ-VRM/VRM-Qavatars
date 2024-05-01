#pragma once

#include "AssetLib/structure/interMeshData.hpp"
#include "AssetLib/structure/modelContext.hpp"
#include <assimp/mesh.h>

namespace AssetLib::Generators
{
    class IntermediateMeshGenerator
    {
        public:
        IntermediateMeshGenerator() = default;
        ~IntermediateMeshGenerator() = default;

        AssetLib::Structure::InterMeshData Generate(aiMesh* mesh, AssetLib::Structure::ModelContext* context, std::optional<AssetLib::Structure::InterMeshData> existingMesh = std::nullopt);
    };
};