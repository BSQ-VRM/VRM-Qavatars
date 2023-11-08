#pragma once

#include "../modelContext.hpp"

namespace AssetLib::Structure::VRM
{
    //Same as ModelContext but contains extra fields for the VRM spec
    class VRMModelContext : virtual public ModelContext
    {
        public:
        VRMModelContext(ModelContext* base)
        {
            originalScene = base->originalScene;
            nodes = base->nodes;
            rootNode = base->rootNode;
            isSkinned = base->isSkinned;
            armature = base->armature;
            rootGameObject = base->rootGameObject;
        }
    };
};