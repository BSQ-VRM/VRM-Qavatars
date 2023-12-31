#include "customTypes/AniLipSync/AnimMorphTarget.hpp"

#include "main.hpp"
#include "UnityEngine/Time.hpp"

namespace VRMQavatars::AniLipSync
{
    CP_SDK_IL2CPP_INHERIT_INIT(AnimMorphTarget);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(AnimMorphTarget)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(AnimMorphTarget)
    {

    }

    void AnimMorphTarget::Start()
    {
        controller = GetComponent<BlendShape::BlendShapeController*>();
        context = GetComponent<OVRLipSync::OVRLipSyncContextBase*>();
        lowLatencyLipSyncContext = static_cast<LowLatencyLipSyncContext*>(context);
    }

    void AnimMorphTarget::Update()
    {
        if (!true)
        {
            return;
        }
        if (context == nullptr)
        {
            return;
        }
        getLogger().info("x1");
        if (frameRate > 0.0f)
        {
            frameRateTimer += UnityEngine::Time::get_deltaTime();
            if (frameRateTimer < 1.0f / frameRate)
            {
                return;
            }
            frameRateTimer -= 1.0f / frameRate;
        }
        getLogger().info("x2");
        ovrLipSyncFrame currentPhonemeFrame = context->GetCurrentPhonemeFrame();
        getLogger().info("x3");
        /*if (currentPhonemeFrame)
        {
            return;
        }*/
        getLogger().info("x4");
        updateFrame.frameNumber = currentPhonemeFrame.frameNumber;
        updateFrame.frameDelay = currentPhonemeFrame.frameDelay;
        updateFrame.visemes = currentPhonemeFrame.visemes;
        updateFrame.laughterScore = currentPhonemeFrame.laughterScore;
        transitionTimer += UnityEngine::Time::get_deltaTime();
        getLogger().info("x5");
        ResetLipSyncBlendShapes();
        getLogger().info("x6");
        int num = 0;
        float num2 = 0.0f;
        //updateFrame.visemes[10] *= PluginConfig.instance.lipSyncWeightAA;
        //updateFrame.visemes[11] *= PluginConfig.instance.lipSyncWeightE;
        //updateFrame.visemes[12] *= PluginConfig.instance.lipSyncWeightIH;
        //updateFrame.visemes[13] *= PluginConfig.instance.lipSyncWeightOH;
        //updateFrame.visemes[14] *= PluginConfig.instance.lipSyncWeightOU;
        getLogger().info("7");
        for (int i = 10; i < updateFrame.visemesLength; i++)
        {
            if (updateFrame.visemes[i] > num2)
            {
                num2 = updateFrame.visemes[i];
                num = i;
            }
        }
        getLogger().info("x8");
        if (num2 * 100.0f < threshold)
        {
            num = static_cast<int>(previousViseme);
        }
        getLogger().info("x9");
        if (lowLatencyLipSyncContext != nullptr && num == 0 && lowLatencyLipSyncContext->GetMicVolume() > rmsThreshold)
        {
            num = static_cast<int>(previousViseme);
        }
        getLogger().info("x10");
        if (previousViseme != static_cast<ovrLipSyncViseme>(num))
        {
            transitionTimer = 0.0f;
            previousViseme = static_cast<ovrLipSyncViseme>(num);
        }
        getLogger().info("x11");
        if (num == 0)
        {
            getLogger().info("x12");
            previousViseme = ovrLipSyncViseme_sil;
            return;
        }
        getLogger().info("x13");
        num2 = std::clamp(num2 * 3.0f, 0.0f, 1.0f);
        int num3 = num - 10;
        getLogger().info("viseme: %d", num3);
        controller->SetBlendshape(viseme2BlendShape[num3], num2 * 100.0f);
    }

    void AnimMorphTarget::ResetLipSyncBlendShapes()
    {
        for (const auto& blendShapePreset : viseme2BlendShape)
        {
            controller->SetBlendshape(blendShapePreset, 0.0f);
        }
    }

}
