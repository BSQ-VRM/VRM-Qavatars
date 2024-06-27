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
        VRMLogger.info("x1");
        if (frameRate > 0.0f)
        {
            frameRateTimer += UnityEngine::Time::get_deltaTime();
            if (frameRateTimer < 1.0f / frameRate)
            {
                return;
            }
            frameRateTimer -= 1.0f / frameRate;
        }
        VRMLogger.info("x2");
        ovrLipSyncFrame currentPhonemeFrame = context->GetCurrentPhonemeFrame();
        VRMLogger.info("x3");
        /*if (currentPhonemeFrame)
        {
            return;
        }*/
        VRMLogger.info("x4");
        updateFrame.frameNumber = currentPhonemeFrame.frameNumber;
        updateFrame.frameDelay = currentPhonemeFrame.frameDelay;
        updateFrame.visemes = currentPhonemeFrame.visemes;
        updateFrame.laughterScore = currentPhonemeFrame.laughterScore;
        transitionTimer += UnityEngine::Time::get_deltaTime();
        VRMLogger.info("x5");
        ResetLipSyncBlendShapes();
        VRMLogger.info("x6");
        int num = 0;
        float num2 = 0.0f;
        //updateFrame.visemes[10] *= PluginConfig.instance.lipSyncWeightAA;
        //updateFrame.visemes[11] *= PluginConfig.instance.lipSyncWeightE;
        //updateFrame.visemes[12] *= PluginConfig.instance.lipSyncWeightIH;
        //updateFrame.visemes[13] *= PluginConfig.instance.lipSyncWeightOH;
        //updateFrame.visemes[14] *= PluginConfig.instance.lipSyncWeightOU;
        VRMLogger.info("7");
        for (int i = 10; i < updateFrame.visemesLength; i++)
        {
            if (updateFrame.visemes[i] > num2)
            {
                num2 = updateFrame.visemes[i];
                num = i;
            }
        }
        VRMLogger.info("x8");
        if (num2 * 100.0f < threshold)
        {
            num = static_cast<int>(previousViseme);
        }
        VRMLogger.info("x9");
        if (lowLatencyLipSyncContext != nullptr && num == 0 && lowLatencyLipSyncContext->GetMicVolume() > rmsThreshold)
        {
            num = static_cast<int>(previousViseme);
        }
        VRMLogger.info("x10");
        if (previousViseme != static_cast<ovrLipSyncViseme>(num))
        {
            transitionTimer = 0.0f;
            previousViseme = static_cast<ovrLipSyncViseme>(num);
        }
        VRMLogger.info("x11");
        if (num == 0)
        {
            VRMLogger.info("x12");
            previousViseme = ovrLipSyncViseme_sil;
            return;
        }
        VRMLogger.info("x13");
        num2 = std::clamp(num2 * 3.0f, 0.0f, 1.0f);
        int num3 = num - 10;
        VRMLogger.info("viseme: {}", num3);
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
