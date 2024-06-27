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
        Blink_R,
        Surprised
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
        int idx;
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
                Blink_R,
            };

            auto master = new VRMBlendShapeMaster();
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
                bSGroup.idx = i;

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

        static void LoadUp(std::vector<VRMBlendShapeGroup>& groups, VRMC_VRM_1_0::Expression expression, const BlendShapePreset blendshape, const int index, const std::string& name)
        {
            auto bSGroup = VRMBlendShapeGroup();

            const std::vector propMappings = {
                "Color",
                "EmissionColor",
                "ShadeColor",
                "RimColor",
                "OutlineColor"
            };

            bSGroup.isBinary = expression.isBinary;
            bSGroup.name = name;
            bSGroup.preset = blendshape;
            bSGroup.binds = std::vector<VRMBlendShapeBind>(expression.morphTargetBinds.size());
            bSGroup.materialBinds = std::vector<VRMBlendshapeMaterialBind>(expression.materialColorBinds.size());
            bSGroup.idx = index;

            for (int k = 0; k < expression.morphTargetBinds.size(); ++k)
            {
                const auto [mesh, index, weight] = expression.morphTargetBinds[k];
                bSGroup.binds[k] = VRMBlendShapeBind(index, mesh, weight);
            }

            for (int k = 0; k < expression.materialColorBinds.size(); ++k)
            {
                const auto [materialName, propertyName, targetValue] = expression.materialColorBinds[k];
                bSGroup.materialBinds[k] = VRMBlendshapeMaterialBind("FUCK", propMappings[static_cast<int>(propertyName)], targetValue);
            }

            groups.push_back(bSGroup);
        }

        static VRMBlendShapeMaster* LoadFromVRM1(const VRMC_VRM_1_0::Vrm& vrm)
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

            const auto master = new VRMBlendShapeMaster();
            auto groups = std::vector<VRMBlendShapeGroup>();

            LoadUp(groups, vrm.expressions.preset.happy, Joy, 0, "Happy");
            LoadUp(groups, vrm.expressions.preset.angry, Angry, 1, "Angry");
            LoadUp(groups, vrm.expressions.preset.sad, Sorrow, 2, "Sad");
            LoadUp(groups, vrm.expressions.preset.relaxed, Fun, 3, "Relaxed");
            LoadUp(groups, vrm.expressions.preset.surprised, Surprised, 4, "Surprised");

            LoadUp(groups, vrm.expressions.preset.aa, A, 5, "aa");
            LoadUp(groups, vrm.expressions.preset.ih, I, 6, "ih");
            LoadUp(groups, vrm.expressions.preset.ou, U, 7, "ou");
            LoadUp(groups, vrm.expressions.preset.ee, E, 8, "ee");
            LoadUp(groups, vrm.expressions.preset.oh, O, 9, "oh");

            LoadUp(groups, vrm.expressions.preset.blink, Blink, 10, "Blink");
            LoadUp(groups, vrm.expressions.preset.blinkLeft, Blink_L, 11, "Blink Left");
            LoadUp(groups, vrm.expressions.preset.blinkRight, Blink_R, 12, "Blink Right");

            LoadUp(groups, vrm.expressions.preset.lookUp, LookUp, 13, "Look Up");
            LoadUp(groups, vrm.expressions.preset.lookDown, LookDown, 14, "Look Down");
            LoadUp(groups, vrm.expressions.preset.lookLeft, LookLeft, 15, "Look Left");
            LoadUp(groups, vrm.expressions.preset.lookRight, LookRight, 16, "Look Right");

            LoadUp(groups, vrm.expressions.preset.neutral, Neutral, 17, "Neutal");

            master->groups = groups;

            return master;
        }
    };
}
