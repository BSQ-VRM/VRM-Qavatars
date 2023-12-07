#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>
#include <sombrero/shared/FastVector2.hpp>
#include <sombrero/shared/FastVector3.hpp>
#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/Vector2.hpp>

#include "VRMSpringBone.hpp"

namespace VRMQavatars
{
    class WindItem
    {
    public:
        WindItem(const Sombrero::FastVector3 orientation, const float riseCount, const float sitCount, const float maxFactor)
        {
            Orientation = orientation;
            RiseCount = riseCount;
            SitCount = sitCount;
            MaxFactor = maxFactor;

            TotalTime = RiseCount + SitCount;

            TimeCount = 0;
        }

        Sombrero::FastVector3 Orientation;
        float RiseCount;
        float SitCount;
        float MaxFactor;
        float TotalTime;
        float TimeCount;

        float CurrentFactor()
        {
            return TimeCount < RiseCount
                        ? MaxFactor * TimeCount / RiseCount
                        : MaxFactor * (1 - (TimeCount - RiseCount) / SitCount);
        }
    };

    class VRMWind : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars", VRMWind, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(VRMWind);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(VRMWind);
    public:

        DECLARE_INSTANCE_METHOD(void, Start);
        DECLARE_INSTANCE_METHOD(void, Update);

    public:
        bool loadAutomatic = true;
        bool enableWind = true;

        Sombrero::FastVector3 windBaseOrientation = Sombrero::FastVector3::get_right();

        float windOrientationRandomPower = 0.2f;

        Sombrero::FastVector2 windStrengthRange = Sombrero::FastVector2(0.03f, 0.06f);
        Sombrero::FastVector2 windIntervalRange = Sombrero::FastVector2(0.7f, 1.9f);
        Sombrero::FastVector2 windRiseCountRange = Sombrero::FastVector2(0.4f, 0.6f);
        Sombrero::FastVector2 windSitCountRange = Sombrero::FastVector2(1.3f, 1.8f);

        float strengthFactor = 1.0f;
        float timeFactor = 1.0f;

        void LoadVRM(UnityEngine::Transform* vrmRoot);
        void UnloadVRM();
        void UpdateWindGenerateCount();
        void UpdateWindItems();
        void DisableWind();

    private:
        float _windGenerateCount = 0;
        std::vector<VRMSpringBone*> _springBones = {};
        std::vector<Sombrero::FastVector3> _originalGravityDirections = {};
        std::vector<float> _originalGravityFactors = {};
        std::vector<WindItem*> _windItems = {};
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::VRMWind);