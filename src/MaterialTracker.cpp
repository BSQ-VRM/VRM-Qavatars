#include "MaterialTracker.hpp"

#include "main.hpp"
#include "MirrorManager.hpp"

namespace VRMQavatars
{
    bool MaterialTracker::bloomEnabled = false;
    std::vector<SafePtrUnity<UnityEngine::Material>> MaterialTracker::materials;

    void MaterialTracker::UpdateMaterials()
    {
        VRMLogger.info("updating materials :3");
        for(auto material : materials)
        {
            VRMLogger.info("updating material");
            if(material)
            {
                VRMLogger.info("valid material {} {}", static_cast<std::string>(material->get_name()).c_str(), static_cast<std::string>(material->shader->get_name()).c_str());
                material->SetInt("_BloomSupport", bloomEnabled ? 1 : 0);
                material->SetInt("_ColorMask", bloomEnabled ? 14 : 15);
            }
        }
        if(MirrorManager::mainMirror)
        {
            MirrorManager::UpdateMirror(false);
        }
    }
}
