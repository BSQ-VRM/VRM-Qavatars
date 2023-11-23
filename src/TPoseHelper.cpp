#include "TPoseHelper.hpp"

#include "UnityEngine/HumanPoseHandler.hpp"

#include "sombrero/shared/FastQuaternion.hpp"

#include "assets.hpp"
#include "json.hpp"
#include "main.hpp"

namespace VRMQavatars
{
    UnityEngine::HumanPose TPoseHelper::LoadPose()
    {
        std::string_view pose = IncludedAssets::tpose_json;
        auto parsed = nlohmann::json::parse(pose);

        auto jsonPose = parsed["clip"];
        auto bodyPos = jsonPose["bodyPosition"];
        auto bodyRot = jsonPose["bodyRotation"];
        std::vector<float> muscles = jsonPose["muscles"];

        const Sombrero::FastVector3 bodyPosVec = {bodyPos["x"], bodyPos["y"], bodyPos["z"]};
        const Sombrero::FastQuaternion bodyPosRot = {bodyRot["x"], bodyRot["y"], bodyRot["z"], bodyRot["w"]};

        const auto humanPose = UnityEngine::HumanPose(bodyPosVec, bodyPosRot, il2cpp_utils::vectorToArray(muscles));
        return humanPose;
    }

    void TPoseHelper::ApplyTpose(UnityEngine::Avatar* avatar, UnityEngine::Transform* root)
    {
        auto humanPose = LoadPose();

        const auto handler = UnityEngine::HumanPoseHandler::New_ctor(avatar, root);

        static auto setHumanPose = il2cpp_utils::resolve_icall<void, UnityEngine::HumanPoseHandler*, ByRef<UnityEngine::Vector3>, ByRef<UnityEngine::Quaternion>, ByRef<ArrayW<float>>>("UnityEngine.HumanPoseHandler::SetHumanPose");
        setHumanPose(handler, humanPose.bodyPosition, humanPose.bodyRotation, humanPose.muscles);
    }
}
