#include "customTypes/GroundOffsetObject.hpp"

#include <chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp>

#include "config/ConfigManager.hpp"

#include "UnityEngine/Transform.hpp"

namespace VRMQavatars
{
    CP_SDK_IL2CPP_INHERIT_INIT(GroundOffsetObject);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(GroundOffsetObject)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(GroundOffsetObject)
    {

    }

    bool approx(const float v1, const float v2)
    {
        return std::abs(v1 - v2) < 0.01f;
    }

    void GroundOffsetObject::Update()
    {
        auto ikConfig = Config::ConfigManager::GetIKSettings();
        if(!approx(ikConfig.groundOffset, get_transform()->get_position().y))
        {
            get_transform()->set_position({0.0f, ikConfig.groundOffset, 0.0f});
        }
    }
}
