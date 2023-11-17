#pragma once
#include <vector>
#include "vrm/vrmIncludes.hpp"

namespace AssetLib::Structure::VRM
{
    struct VRMBlendShapeBind
    {
        int index;
        int mesh;
        float weight;
    };

    struct VRMBlendshapeMaterialbind {
        std::string materialName;
        std::string propertyName;
        std::vector<float> targetValue;
    };

    struct VRMBlendShapeGroup
    {
        std::vector<VRMBlendShapeBind> binds;
        bool isBinary;
        std::vector<VRMBlendshapeMaterialbind> materialBinds;
        std::string name;
        std::string presetName;
    };

    class VRMBlendShapeMaster
    {
    public:
        std::vector<VRMBlendShapeGroup> groups;

        static VRMBlendShapeMaster* LoadFromVRM0(VRMC_VRM_0_0::Vrm vrm)
        {
            const std::vector<std::string> PresetNameMappings = {
                "Unknown",
                "Neutral",
                "A",
                "I",
                "U",
                "E",
                "O",
                "Blink",
                "Joy",
                "Angry",
                "Sorrow",
                "Fun",
                "Lookup",
                "Lookdown",
                "Lookleft",
                "Lookright",
                "Blink_l",
                "Blink_r"
            };

            VRMBlendShapeMaster* master = new VRMBlendShapeMaster();
            master->groups = std::vector<VRMBlendShapeGroup>(vrm.blendShapeMaster.blendShapeGroups.size());
            for (int i = 0; i < vrm.blendShapeMaster.blendShapeGroups.size(); ++i)
            {
                const auto [name, presetName, binds, materialValues, isBinary] = vrm.blendShapeMaster.blendShapeGroups[i];
                auto bSGroup = VRMBlendShapeGroup();

                bSGroup.isBinary = isBinary;
                bSGroup.name = name;
                bSGroup.presetName = PresetNameMappings[static_cast<int>(presetName)];
                bSGroup.binds = std::vector<VRMBlendShapeBind>(binds.size());
                bSGroup.materialBinds = std::vector<VRMBlendshapeMaterialbind>(materialValues.size());

                for (int k = 0; k < binds.size(); ++k)
                {
                    const auto [mesh, index, weight] = binds[k];
                    bSGroup.binds[k] = VRMBlendShapeBind(index, mesh, weight);
                }

                for (int k = 0; k < materialValues.size(); ++k)
                {
                    const auto [materialName, propertyName, targetValue] = materialValues[k];
                    bSGroup.materialBinds[k] = VRMBlendshapeMaterialbind(materialName, propertyName, targetValue);
                }

                master->groups[i] = bSGroup;
            }
            return master;
        }
    };
}
