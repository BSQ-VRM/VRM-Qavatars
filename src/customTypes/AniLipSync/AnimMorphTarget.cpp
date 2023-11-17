#include "customTypes/AniLipSync/AnimMorphTarget.hpp"

#include "UnityEngine/Time.hpp"

namespace VRMQavatars::AniLipSync
{
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
        if (frameRate > 0.0f)
        {
            frameRateTimer += UnityEngine::Time::get_deltaTime();
            if (frameRateTimer < 1.0f / frameRate)
            {
                return;
            }
            frameRateTimer -= 1.0f / frameRate;
        }
        ovrLipSyncFrame currentPhonemeFrame = context->GetCurrentPhonemeFrame();
        /*if (currentPhonemeFrame)
        {
            return;
        }*/
        updateFrame.frameNumber = currentPhonemeFrame.frameNumber;
        updateFrame.frameDelay = currentPhonemeFrame.frameDelay;
        updateFrame.visemes = currentPhonemeFrame.visemes;
        updateFrame.laughterScore = currentPhonemeFrame.laughterScore;
        transitionTimer += UnityEngine::Time::get_deltaTime();
        ResetLipSyncBlendShapes();
        int num = 0;
        float num2 = 0.0f;
        //updateFrame.visemes[10] *= PluginConfig.instance.lipSyncWeightAA;
        //updateFrame.visemes[11] *= PluginConfig.instance.lipSyncWeightE;
        //updateFrame.visemes[12] *= PluginConfig.instance.lipSyncWeightIH;
        //updateFrame.visemes[13] *= PluginConfig.instance.lipSyncWeightOH;
        //updateFrame.visemes[14] *= PluginConfig.instance.lipSyncWeightOU;
        for (int i = 10; i < updateFrame.visemesLength; i++)
        {
            if (updateFrame.visemes[i] > num2)
            {
                num2 = updateFrame.visemes[i];
                num = i;
            }
        }
        if (num2 * 100.0f < threshold)
        {
            num = static_cast<int>(previousViseme);
        }
        if (lowLatencyLipSyncContext != nullptr && num == 0 && lowLatencyLipSyncContext->GetMicVolume() > rmsThreshold)
        {
            num = static_cast<int>(previousViseme);
        }
        if (previousViseme != static_cast<ovrLipSyncViseme>(num))
        {
            transitionTimer = 0.0f;
            previousViseme = static_cast<ovrLipSyncViseme>(num);
        }
        if (num == 0)
        {
            previousViseme = ovrLipSyncViseme_sil;
            return;
        }
        num2 = std::clamp(num2 * 3.0f, 0.0f, 1.0f);
        int num3 = num - 10;
    }

    void AnimMorphTarget::ResetLipSyncBlendShapes()
    {

    }

}
