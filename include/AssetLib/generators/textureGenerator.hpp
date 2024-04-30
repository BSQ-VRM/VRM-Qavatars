#pragma once

#include "UnityEngine/Texture2D.hpp"
#include "json.hpp"

namespace AssetLib::Generators
{
    class TextureGenerator
    {
        public:
        static void Generate(ArrayW<uint8_t>* textures, int num, const std::function<void(UnityEngine::Texture2D**)>& onFinish);
    };
};