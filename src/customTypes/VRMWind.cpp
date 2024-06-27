#include "customTypes/VRMWind.hpp"

#include <chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp>
#include <UnityEngine/Random.hpp>

#include "config/ConfigManager.hpp"

namespace VRMQavatars
{
    CP_SDK_IL2CPP_INHERIT_INIT(VRMWind);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(VRMWind)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(VRMWind)
    {

    }

    void VRMWind::Start()
    {
        if (loadAutomatic)
        {
            LoadVRM(get_transform());
        }
    }

    void VRMWind::Update()
    {
        if (!enableWind)
        {
            return;
        }

        UpdateWindGenerateCount();
        UpdateWindItems();

        Sombrero::FastVector3 windForce = {0,0,0};
        for (int i = 0; i < _windItems.size(); i++)
        {
            const auto x = (_windItems[i]->Orientation * _windItems[i]->CurrentFactor());
            windForce += x;
        }

        for (int i = 0; i < _springBones.size(); i++)
        {
            const auto bone = _springBones[i];
            Sombrero::FastVector3 forceSum = _originalGravityDirections[i] * _originalGravityFactors[i] + windForce;
            bone->gravityDir = forceSum.get_normalized();
            bone->gravityPower = forceSum.get_magnitude();
        }
    }

    void VRMWind::UpdateWindItems()
    {
        for (int i = _windItems.size() - 1; i >= 0; i--)
        {
            const auto item = _windItems[i];
            item->TimeCount += UnityEngine::Time::get_deltaTime();
            if (item->TimeCount >= item->TotalTime)
            {
                _windItems.erase(_windItems.begin() + i);
            }
        }
    }

    void VRMWind::UnloadVRM()
    {
        _springBones = { };
        _originalGravityDirections = { };
        _originalGravityFactors = { };
    }

    void VRMWind::LoadVRM(UnityEngine::Transform* vrmRoot)
    {
        for (auto bone : vrmRoot->GetComponentsInChildren<VRMSpringBone*>())
        {
            _springBones.push_back(bone);
            _originalGravityDirections.push_back(bone->gravityDir);
            _originalGravityFactors.push_back(bone->gravityPower);
        }
    }

    void VRMWind::UpdateWindGenerateCount()
    {
        _windGenerateCount -= UnityEngine::Time::get_deltaTime();
        if (_windGenerateCount > 0)
        {
            return;
        }
        _windGenerateCount = UnityEngine::Random::Range(windIntervalRange.x, windIntervalRange.y) * timeFactor;

        const auto windOrientation = (
            windBaseOrientation.get_normalized() +
            Sombrero::FastVector3(
               UnityEngine::Random::Range(-windOrientationRandomPower, windOrientationRandomPower),
               UnityEngine::Random::Range(-windOrientationRandomPower, windOrientationRandomPower),
               UnityEngine::Random::Range(-windOrientationRandomPower, windOrientationRandomPower)
                )).get_normalized();

        _windItems.push_back(new WindItem(
            windOrientation,
            UnityEngine::Random::Range(windRiseCountRange.x, windRiseCountRange.y),
            UnityEngine::Random::Range(windSitCountRange.x, windSitCountRange.y),
            UnityEngine::Random::Range(windStrengthRange.x, windStrengthRange.y) * strengthFactor
        ));
    }

    void VRMWind::DisableWind()
    {
        for (int i = 0; i < _springBones.size(); i++)
        {
            const auto bone = _springBones[i];
            bone->gravityDir = _originalGravityDirections[i];
            bone->gravityPower = _originalGravityFactors[i];
        }
    }

}
