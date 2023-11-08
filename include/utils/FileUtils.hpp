#pragma once
#include <dirent.h>

//From Qosmetics

std::string_view GetExtension(std::string_view path)
{
    int pos = path.find_last_of(".");
    if (pos != std::string::npos)
        return path.substr(pos + 1);
    return "";
}

std::string_view GetFileName(std::string_view path)
{
    int pos = path.find_last_of("/");
    if (pos != std::string::npos)
        return path.substr(pos + 1);
    else
        return path;
}

bool GetFilesInFolderPath(std::string_view extension, std::string_view filePath, std::vector<std::string>& out)
{
    bool foundTheExtension = false;
    struct DIR* fileDir = opendir(filePath.data());
    struct dirent* files;
    if (fileDir != NULL)
    {
        while ((files = readdir(fileDir)) != NULL)
        {
            char* fileName = files->d_name;
            if (!strcmp(fileName, ".") || !strcmp(fileName, ".."))
                continue;
            if (GetExtension(fileName) == extension)
            {
                out.push_back(fileName);
                foundTheExtension = true;
            }
        }
        closedir(fileDir);
    }
    return foundTheExtension;
}