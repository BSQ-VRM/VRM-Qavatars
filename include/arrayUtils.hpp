#pragma once
#include "main.hpp"

namespace ArrayUtils {

    template <typename T>
    ArrayW<T> vector2ArrayW(std::vector<T> vector)
    {
        ArrayW<T> arr = ArrayW<T>(vector.size());
        for (int i = 0; i < vector.size(); i++)
        {
            arr[i] = vector[i];
        }
        return arr;
    }
}