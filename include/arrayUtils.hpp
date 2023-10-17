#pragma once
#include "main.hpp"

namespace ArrayUtils {
    template <typename T>
    void allocateInto2DArray(int x, int y, std::vector<std::vector<T>>& vector)
    {
        vector = std::vector<std::vector<T>>(x);
        for (int i = 0; i < x; i++)
        {
            vector[i] = std::vector<T>(y);
        }
    }

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