#pragma once
#include <vector>

#include "vrm/vrmIncludes.hpp"

namespace AssetLib::Structure::VRM
{
    enum BlendShapePreset
    {
        Unknown,
        Neutral,
        A,
        I,
        U,
        E,
        O,
        Blink,
        Joy,
        Angry,
        Sorrow,
        Fun,
        LookUp,
        LookDown,
        LookLeft,
        LookRight,
        Blink_L,
        Blink_R
    };

    struct VRMBlendShapeBind
    {
        VRMBlendShapeBind(const int index, const int mesh, const float weight)
            : index(index),
              mesh(mesh),
              weight(weight)
        {
        }

        VRMBlendShapeBind() = default;

        int index;
        int mesh;
        float weight;
    };

    struct VRMBlendshapeMaterialBind {
        VRMBlendshapeMaterialBind(const std::string& material_name, const std::string& property_name,
            const std::vector<float>& target_value)
            : materialName(material_name),
              propertyName(property_name),
              targetValue(target_value)
        {
        }
        VRMBlendshapeMaterialBind() = default;

        std::string materialName;
        std::string propertyName;
        std::vector<float> targetValue;
    };

    struct VRMBlendShapeGroup
    {
        std::vector<VRMBlendShapeBind> binds;
        bool isBinary;
        std::vector<VRMBlendshapeMaterialBind> materialBinds;
        std::string name;
        BlendShapePreset preset;
    };

    class VRMBlendShapeMaster
    {
    public:
        std::vector<VRMBlendShapeGroup> groups;

        static VRMBlendShapeMaster* LoadFromVRM0(VRMC_VRM_0_0::Vrm vrm)
        {
            const std::vector PresetNameMappings = {
                Unknown,
                Neutral,
                A,
                I,
                U,
                E,
                O,
                Blink,
                Joy,
                Angry,
                Sorrow,
                Fun,
                LookUp,
                LookDown,
                LookLeft,
                LookRight,
                Blink_L,
                Blink_R
            };

            VRMBlendShapeMaster* master = new VRMBlendShapeMaster();
            master->groups = std::vector<VRMBlendShapeGroup>(vrm.blendShapeMaster.blendShapeGroups.size());
            for (int i = 0; i < vrm.blendShapeMaster.blendShapeGroups.size(); ++i)
            {
                const auto [name, presetName, binds, materialValues, isBinary] = vrm.blendShapeMaster.blendShapeGroups[i];
                auto bSGroup = VRMBlendShapeGroup();

                bSGroup.isBinary = isBinary;
                bSGroup.name = name;
                bSGroup.preset = PresetNameMappings[static_cast<int>(presetName)];
                bSGroup.binds = std::vector<VRMBlendShapeBind>(binds.size());
                bSGroup.materialBinds = std::vector<VRMBlendshapeMaterialBind>(materialValues.size());

                for (int k = 0; k < binds.size(); ++k)
                {
                    const auto [mesh, index, weight] = binds[k];
                    bSGroup.binds[k] = VRMBlendShapeBind(index, mesh, weight);
                }

                for (int k = 0; k < materialValues.size(); ++k)
                {
                    const auto [materialName, propertyName, targetValue] = materialValues[k];
                    bSGroup.materialBinds[k] = VRMBlendshapeMaterialBind(materialName, propertyName, targetValue);
                }

                master->groups[i] = bSGroup;
            }
            return master;
        }
    };
}
