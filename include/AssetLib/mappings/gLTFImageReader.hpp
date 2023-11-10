#pragma once

#include "main.hpp"
#include "json.hpp"
#include <fstream>

#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/ImageConversion.hpp"

namespace VRMQavatars
{
    class gLTFImageReader 
    {
        public:
        static UnityEngine::Texture2D* ReadImage(nlohmann::json bufferView, uint32_t jsonLength, std::ifstream& binFile)
        {
            uint32_t size = bufferView["byteLength"].get<uint32_t>();
            uint32_t start = bufferView["byteOffset"].get<uint32_t>();
            std::string thing;
            thing.resize(size);
            binFile.seekg((28 + jsonLength) + start);
            binFile.read(thing.data(), size);

            auto ret = ArrayW<uint8_t>(thing.size());
            for (size_t i = 0; i < thing.size(); i++)
            {
                ret[i] = thing.data()[i];
            }

            UnityEngine::Texture2D* texture = UnityEngine::Texture2D::New_ctor(0, 0, UnityEngine::TextureFormat::RGBA32, false, false);
            UnityEngine::ImageConversion::LoadImage(texture, ret, false);

            return texture;
        }

        static UnityEngine::Texture2D* ReadImageIndex(uint32_t jsonLength, std::ifstream& binFile, int imageIndex)
        {
            std::string jsonStr;
            jsonStr.resize(jsonLength);
            binFile.seekg(20); // Skip the rest of the JSON header to the start of the string
            binFile.read(jsonStr.data(), jsonLength); // Read out the json string

            auto doc = nlohmann::json::parse(jsonStr);

            auto images = doc["images"];
            auto bufferViews = doc["bufferViews"];

            auto img = images[imageIndex];
            auto bufferView = bufferViews[img["bufferView"].get<uint32_t>()];

            uint32_t size = bufferView["byteLength"].get<uint32_t>();
            uint32_t start = bufferView["byteOffset"].get<uint32_t>();
            std::string thing;
            thing.resize(size);
            binFile.seekg((28 + jsonLength) + start);
            binFile.read(thing.data(), size);

            auto ret = ArrayW<uint8_t>(thing.size());
            for (size_t i = 0; i < thing.size(); i++)
            {
                ret[i] = thing.data()[i];
            }

            UnityEngine::Texture2D* texture = UnityEngine::Texture2D::New_ctor(0, 0, UnityEngine::TextureFormat::RGBA32, false, false);
            UnityEngine::ImageConversion::LoadImage(texture, ret, false);

            return texture;
        }
    };
};