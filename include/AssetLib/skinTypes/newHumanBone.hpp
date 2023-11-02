#pragma once
#include "UnityEngine/HumanLimit.hpp"

//Exists because codegen machine broke
struct CustomHumanBone {
    CustomHumanBone() = default;
    CustomHumanBone(StringW bN, StringW hN, UnityEngine::HumanLimit hL) {
        boneName = bN;
        humanName = hN;
        humanLimit = hL;
    }
    StringW boneName; // offset 0
    StringW humanName; // offset 0x8
    // padding here? idk, check offsets in 
    UnityEngine::HumanLimit humanLimit; 

    //5 Hours of debugging later
    uint8_t pad[4]; // offset 0x3a, pad to 0x40
};
static_assert(sizeof(CustomHumanBone) == 0x40);
DEFINE_IL2CPP_ARG_TYPE(CustomHumanBone, "UnityEngine", "HumanBone");
