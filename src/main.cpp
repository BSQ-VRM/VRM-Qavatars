#include "main.hpp"
#include "arrayUtils.hpp"

#include <fstream>

#include "gltf/UnityMeshData.hpp"

#include "assimp/Importer.hpp"
#include "assimp/Exporter.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"

#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/SkinnedMeshRenderer.hpp"
#include "UnityEngine/Rendering/IndexFormat.hpp"
#include "UnityEngine/Matrix4x4.hpp"

static ModInfo modInfo;

Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

inline bool exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

std::vector<UnityEngine::Transform*> boneObjs;

void constructUnityMesh(const char* name, UnityMeshData meshData, aiMesh* assimpMesh, UnityEngine::SkinnedMeshRenderer* renderer)
{
    UnityEngine::Mesh* unityMesh = UnityEngine::Mesh::New_ctor();
    unityMesh->set_name(name);
    unityMesh->set_indexFormat(meshData.vertices.size() > 65535 ? UnityEngine::Rendering::IndexFormat::UInt32 : UnityEngine::Rendering::IndexFormat::UInt16);
    
    getLogger().info("Mesh object created, setting values");

    unityMesh->set_vertices(ArrayUtils::vector2ArrayW(meshData.vertices));
    unityMesh->set_normals(ArrayUtils::vector2ArrayW(meshData.normals));
    unityMesh->set_tangents(ArrayUtils::vector2ArrayW(meshData.tangents));
    unityMesh->set_uv(ArrayUtils::vector2ArrayW(meshData.uv1));
    unityMesh->set_uv2(ArrayUtils::vector2ArrayW(meshData.uv2));
    unityMesh->set_uv3(ArrayUtils::vector2ArrayW(meshData.uv3));
    unityMesh->set_uv4(ArrayUtils::vector2ArrayW(meshData.uv4));
    unityMesh->set_colors(ArrayUtils::vector2ArrayW(meshData.colors));

    /*std::vector<UnityEngine::BoneWeight> weights;
    for (size_t i = 0; i < assimpMesh->mNumBones; i++)
    {
        assimpMesh->mBones[i]->mOffsetMatrix
    }*/

    getLogger().info("loading bones");
    
    std::vector<UnityEngine::BoneWeight> convertedBW = std::vector<UnityEngine::BoneWeight>(meshData.boneWeights.size());

    for (size_t i = 0; i < meshData.boneWeights.size(); i++)
    {
        getLogger().info("%zu", i);
        convertedBW[i] = meshData.boneWeights[i].convert();
    }

    getLogger().info("loaded bones");
    
    unityMesh->set_boneWeights(ArrayUtils::vector2ArrayW(convertedBW));

    unityMesh->set_subMeshCount(meshData.indices.size());
    uint baseVertex = 0;
	for (int i = 0; i < meshData.indices.size(); i++)
	{
		unityMesh->SetIndices(ArrayW<int>(ArrayUtils::vector2ArrayW(meshData.indices[i])), meshData.topology[i], i, false, (int)baseVertex);
		//baseVertex += getVertexCount(mesh.primitives[i], model);
	}
    unityMesh->RecalculateBounds();

    //Support blendshapes one day
    /*if (meshData.morphTargetVertices.size() > 0)
	{
		for (int i = 0; i < meshData.morphTargetVertices.size(); i++)
		{
		    auto targetName = "Morphtarget " + std::to_string(i);
			unityMesh->AddBlendShapeFrame(targetName, 100,
				ArrayUtils::vector2ArrayW(meshData.morphTargetVertices[i]),
				meshData.morphTargetNormals.size() > 0 ? ArrayUtils::vector2ArrayW(meshData.morphTargetNormals[i]) : nullptr,
				meshData.morphTargetTangents.size() > 0 ? ArrayUtils::vector2ArrayW(meshData.morphTargetTangents[i]) : nullptr
			);
		}
	}*/

	if (meshData.normals.size() < 1 && meshData.topology[0] == UnityEngine::MeshTopology::Triangles)
	{
		unityMesh->RecalculateNormals();
	}

    std::vector<UnityEngine::Matrix4x4> bindPoses = std::vector<UnityEngine::Matrix4x4>(boneObjs.size());

    for (size_t i = 0; i < boneObjs.size(); i++)
    {
        bindPoses[i] = boneObjs[i]->get_worldToLocalMatrix() * renderer->get_transform()->get_localToWorldMatrix();
    }

    unityMesh->set_bindposes(ArrayUtils::vector2ArrayW(bindPoses));
    
    renderer->set_sharedMesh(unityMesh);

    renderer->set_rootBone(boneObjs.front());
    renderer->set_bones(ArrayUtils::vector2ArrayW(boneObjs));
    
    getLogger().info("mesh components setup");
}

UnityMeshData loadMesh(aiMesh* mesh)
{
    getLogger().info("x");
    UnityMeshData meshData;
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        auto vert = mesh->mVertices[i];
        meshData.vertices.push_back(UnityEngine::Vector3(vert.x, vert.y, vert.z));
    }
    getLogger().info("x1");
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        auto norm = mesh->mNormals[i];
        meshData.normals.push_back(UnityEngine::Vector3(norm.x, norm.y, norm.z));
    }
    getLogger().info("x2");
    if (mesh->mTangents != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto tang = mesh->mTangents[i];
            meshData.tangents.push_back(UnityEngine::Vector4(tang.x, tang.y, tang.z, 0.0f));
        }
    }
    getLogger().info("x3");
    if (mesh->mTextureCoords[0] != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto tex = mesh->mTextureCoords[0][i];
            meshData.uv1.push_back(UnityEngine::Vector2(tex.x, tex.y));
        }
    }
    if (mesh->mTextureCoords[1] != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto tex = mesh->mTextureCoords[1][i];
            meshData.uv2.push_back(UnityEngine::Vector2(tex.x, tex.y));
        }
    }
    if (mesh->mTextureCoords[2] != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto tex = mesh->mTextureCoords[2][i];
            meshData.uv3.push_back(UnityEngine::Vector2(tex.x, tex.y));
        }
    }
    if (mesh->mTextureCoords[3] != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto tex = mesh->mTextureCoords[3][i];
            meshData.uv4.push_back(UnityEngine::Vector2(tex.x, tex.y));
        }
    }
    getLogger().info("x4");
    if (mesh->mColors[0] != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto col = mesh->mColors[0][i];
            meshData.colors.push_back(UnityEngine::Color(col.r, col.g, col.b, col.a));
        }
    }
    meshData.indices.push_back(std::vector<int>(0));
    getLogger().info("x5");
    meshData.topology.push_back(UnityEngine::MeshTopology::Triangles);
    if (mesh->mFaces != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            auto face = mesh->mFaces[i];
            for (size_t x = 0; x < face.mNumIndices; x++)
            {
                meshData.indices[0].emplace_back(face.mIndices[x]);
            }
        }
    }

    meshData.boneWeights = std::vector<BoneWeightProxy>(mesh->mNumVertices);

    for (uint i = 0 ; i < mesh->mNumBones ; i++) {
        uint BoneIndex = 0;
        std::string BoneName(mesh->mBones[i]->mName.data);

        for (uint j = 0 ; j < mesh->mBones[i]->mNumWeights ; j++) {
            //save for submeshing?
            uint VertexID = /*m_Entries[MeshIndex].BaseVertex*/ + mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = mesh->mBones[i]->mWeights[j].mWeight;
            meshData.boneWeights[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
    
    getLogger().info("x6");
    return meshData;
}

int test = -2;
void logNode(UnityEngine::Transform* parentObj, aiNode* node, int depth, bool isBones)
{
    test++;

    bool nextBones = isBones;

    //TODO: proper solution AAAAAAAAA, requires that root bone obj is named Root plus that the meshes always come after bones
    if(strcmp(node->mName.C_Str(), "Root") == 0) nextBones = true;

    auto pos = node->mTransformation;

    UnityEngine::Transform* obj;
    if(node->mNumMeshes > 0)
    {
        obj = UnityEngine::GameObject::New_ctor()->get_transform();
        obj->get_gameObject()->set_name(node->mName.C_Str());
        obj->SetParent(parentObj, false);
        obj->set_localPosition(UnityEngine::Vector3(pos.a4, pos.b4, pos.c4));
    }
    else
    {
        obj = UnityEngine::GameObject::CreatePrimitive(UnityEngine::PrimitiveType::Sphere)->get_transform();
        obj->get_gameObject()->set_name(node->mName.C_Str());
        obj->SetParent(parentObj, false);
        obj->set_localPosition(UnityEngine::Vector3(pos.a4, pos.b4, pos.c4) * 60.0f);
    }

    if(nextBones)
    {
        boneObjs.push_back(obj);
    }

    getLogger().info("%d     %s%s children: %d  locpos: %f,%f,%f", test, std::string(depth, '-').c_str(), node->mName.C_Str(), node->mNumChildren, pos.a4, pos.b4, pos.c4);
    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        logNode(obj, node->mChildren[i], depth + 1, nextBones);
    }
}
 
MAKE_HOOK_MATCH(MainMenuUIHook, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuUIHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    getLogger().info("Starting Load!");

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile("sdcard/ModData/ava.vrm", aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_PopulateArmatureData);
    getLogger().info("Imported");
    
    auto Root = UnityEngine::GameObject::New_ctor("GLTF");
    UnityEngine::GameObject::DontDestroyOnLoad(Root);
    Root->get_transform()->set_position(UnityEngine::Vector3(0.0f, 0.0f, 0.0f));
    Root->get_transform()->set_localScale(UnityEngine::Vector3(0.02f, 0.02f, 0.02f));

    getLogger().info("Created root object");

    auto rootNode = scene->mRootNode;
    logNode(Root->get_transform(), rootNode, 0, false);
}

extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
    
    getConfig().Load();
}

extern "C" void load() {
    il2cpp_functions::Init();

    INSTALL_HOOK(getLogger(), MainMenuUIHook);
}