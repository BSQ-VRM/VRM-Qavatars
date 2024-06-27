#include "AssetLib/generators/materialGenerator.hpp"
#include "AssetLib/modelImporter.hpp"

namespace AssetLib::Generators
{
    template<>
    UnityEngine::Material* MaterialGenerator::Generate(const VRMC_VRM_0_0::Vrm& vrm, int index, UnityEngine::Texture2D** textures)
    {
        auto [name, shader, renderQueue, floatProperties, vectorProperties, textureProperties, keywordMap, tagMap] = vrm.materialProperties[index];

        auto mat = UnityEngine::Material::New_ctor(AssetLib::ModelImporter::mtoon.ptr());

        for (const auto & [ key, value ] : floatProperties )
        {
            mat->SetFloat(key.c_str(), value);
        }

        for (const auto & [ key, value ] : keywordMap )
        {
            if(value)
            {
                mat->EnableKeyword(key.c_str());
            }
            else
            {
                mat->DisableKeyword(key.c_str());
            }

        }

        for (const auto & [ key, value ] : textureProperties )
        {
            mat->SetTexture(key.c_str(), textures[value]);
        }

        for (const auto & [ key, value ] : vectorProperties )
        {
            mat->SetColor(key.c_str(), Sombrero::FastColor(value[0], value[1], value[2], value[3]));
        }

        for (const auto & [ key, value ] : tagMap )
        {
            mat->SetOverrideTag(key, value);
        }

        mat->set_renderQueue(static_cast<int32_t>(renderQueue));

        return mat;
    }

    template<>
    UnityEngine::Material* MaterialGenerator::Generate(const VRMC_VRM_1_0::Vrm& vrm, int index, UnityEngine::Texture2D** textures)
    {
        //TODO: Implement VRMC_VRM_1_0::Vrm material generation
        return nullptr;
    }
}