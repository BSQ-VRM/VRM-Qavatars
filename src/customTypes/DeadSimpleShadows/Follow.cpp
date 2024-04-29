#include "customTypes/DeadSimpleShadows/Follow.hpp"

#include "chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp"
#include "main.hpp"
#include "sombrero/shared/Vector3Utils.hpp"

#include "UnityEngine/Transform.hpp"

namespace DeadSimpleShadows
{
    CP_SDK_IL2CPP_INHERIT_INIT(Follow);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(Follow)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(Follow)
    {

    }

    void Follow::Update()
    {
        if (!useMeshBoundingBox || !drawSystem)
        {
            if (target == nullptr) return;
            transform->position = Sombrero::FastVector3(target->position) - (Sombrero::FastVector3(transform->forward) * 6.0f);
        }
        else
        {
            auto bounds = drawSystem->GetIdealBoundingBox();
            transform->position = Sombrero::FastVector3(bounds.center) - (Sombrero::FastVector3(transform->forward) * 6.0f);
        }
    }
}