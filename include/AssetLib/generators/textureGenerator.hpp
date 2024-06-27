#pragma once

#include "UnityEngine/Texture2D.hpp"
#include "json.hpp"

namespace AssetLib::Generators
{
    class TextureGenerator
    {
        public:
        TextureGenerator() = default;
        ~TextureGenerator() = default;

        UnityEngine::Texture2D** Generate(ArrayW<uint8_t>* textures, int num);
    };
};