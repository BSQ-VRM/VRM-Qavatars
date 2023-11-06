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

    //borrowed from QuestUI
    template <class Out, class T, class Predicate>
    inline std::vector<Out> Select(std::vector<T> array, Predicate pred)
    {
        std::vector<Out> newArray(array.size()); 
        for (int i = 0; i < array.size(); i++) { 
            newArray[i] = pred(array[i]); 
        } 
        return newArray;
    }
}