#include "AssetLib/generators/textureGenerator.hpp"

#include "AssetLib/gLTFImageReader.hpp"

#include "bsml/shared/BSML/MainThreadScheduler.hpp"

#include "UnityEngine/TextureFormat.hpp"
#include "UnityEngine/Texture2D.hpp"

#include <cstdint>
#include <functional>

namespace AssetLib::Generators
{
    void TextureGenerator::Generate(ArrayW<uint8_t>* textures, int num, const std::function<void(UnityEngine::Texture2D**)>& onFinish)
    {   
        auto** retTextures = new UnityEngine::Texture2D*[num];

        for (int i = 0; i < num; i++)
        {
            retTextures[i] = UnityEngine::Texture2D::New_ctor(0, 0, UnityEngine::TextureFormat::RGBA32, false, false);
            UnityEngine::ImageConversion::LoadImage(retTextures[i], textures[i], false);
        }

        onFinish(retTextures);
    }
}