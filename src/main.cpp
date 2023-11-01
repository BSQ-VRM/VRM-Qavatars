#include "main.hpp"
#include "arrayUtils.hpp"
#include "shaderLoader.hpp"
#include "ShaderSO.hpp"

#include <fstream>

#include "UnityMeshData.hpp"

#include "assimp/Importer.hpp"
#include "assimp/Exporter.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"

#include "UnityEngine/Light.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/SkinnedMeshRenderer.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Rendering/IndexFormat.hpp"
#include "UnityEngine/Matrix4x4.hpp"
#include "UnityEngine/Animator.hpp"
#include "UnityEngine/HumanBodyBones.hpp"

#include "questui/shared/ArrayUtil.hpp"

#include "RootMotion/FinalIK/VRIK.hpp"
#include "RootMotion/FinalIK/IKSolverVR.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Arm.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Spine.hpp"
#include "RootMotion/FinalIK/VRIK_References.hpp"

#include "vrm/mappings/avatarMappings.hpp"

#include "json.hpp"

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

void constructUnityMesh(const char* name, UnityMeshData meshData, UnityEngine::SkinnedMeshRenderer* renderer)
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

    getLogger().info("loading bones");
    
    std::vector<UnityEngine::BoneWeight> convertedBW = std::vector<UnityEngine::BoneWeight>(meshData.boneWeights.size());

    for (size_t i = 0; i < meshData.boneWeights.size(); i++)
    {
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

    getLogger().info("%lu", convertedBW.size());
    getLogger().info("%lu", boneObjs.size());
    getLogger().info("%lu", bindPoses.size());
    getLogger().info("%s", static_cast<std::string>(boneObjs.front()->get_gameObject()->get_name()).c_str());
    unityMesh->set_bindposes(ArrayUtils::vector2ArrayW(bindPoses));
    
    renderer->set_sharedMesh(unityMesh);

    renderer->set_rootBone(boneObjs.front());
    renderer->set_bones(ArrayUtils::vector2ArrayW(boneObjs));
    
    getLogger().info("mesh components setup");
}

UnityMeshData loadMesh(aiMesh* mesh)
{
    UnityMeshData meshData;
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        auto vert = mesh->mVertices[i];
        meshData.vertices.push_back(UnityEngine::Vector3(vert.x, vert.y, vert.z));
    }
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        auto norm = mesh->mNormals[i];
        meshData.normals.push_back(UnityEngine::Vector3(norm.x, norm.y, norm.z));
    }
    if (mesh->mTangents != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto tang = mesh->mTangents[i];
            meshData.tangents.push_back(UnityEngine::Vector4(tang.x, tang.y, tang.z, 0.0f));
        }
    }
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
    if (mesh->mColors[0] != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto col = mesh->mColors[0][i];
            meshData.colors.push_back(UnityEngine::Color(col.r, col.g, col.b, col.a));
        }
    }
    meshData.indices.push_back(std::vector<int>(0));
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
        std::string BoneName(mesh->mBones[i]->mName.data);
        uint BoneIndex = i;

        for (uint j = 0 ; j < mesh->mBones[i]->mNumWeights ; j++) {
                            //save for submeshing?
            uint VertexID = /*m_Entries[MeshIndex].BaseVertex*/ + mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = mesh->mBones[i]->mWeights[j].mWeight;
            meshData.boneWeights[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
    
    return meshData;
}

static SafePtrUnity<UnityEngine::Material> body = nullptr;
static SafePtrUnity<UnityEngine::Material> hair = nullptr;
static SafePtrUnity<UnityEngine::Material> detail = nullptr;
static SafePtrUnity<UnityEngine::Material> face = nullptr;

void PrewarmAllShadersOnMaterial(UnityEngine::Material* mat)
{
    if (!mat)
        return;

    // all shader variant stuff is stripped so resolve icall
    static auto createFunc = reinterpret_cast<function_ptr_t<void, Il2CppObject*>>(il2cpp_functions::resolve_icall("UnityEngine.ShaderVariantCollection::Internal_Create"));
    static auto addFunc = reinterpret_cast<function_ptr_t<bool, Il2CppObject*, Il2CppObject*, int, ArrayW<StringW>>>(il2cpp_functions::resolve_icall("UnityEngine.ShaderVariantCollection::AddVariant"));
    static auto warmupFunc = reinterpret_cast<function_ptr_t<void, Il2CppObject*>>(il2cpp_functions::resolve_icall("UnityEngine.ShaderVariantCollection::WarmUp"));
    Il2CppObject* obj = UnityEngine::Object::New_ctor();
    createFunc(obj);

    addFunc(obj, mat->get_shader(), 0, mat->get_shaderKeywords());

    warmupFunc(obj);
}

int test = -2;
void logNode(UnityEngine::Transform* parentObj, aiNode* node, int depth, bool isBones, const aiScene* scene, aiMatrix4x4 parentTransform)
{
    test++;

    bool nextBones = isBones;

    auto pos = node->mTransformation * parentTransform;

    UnityEngine::Transform* obj;
    if(node->mNumMeshes > 0)
    {
        //unideal because this breaks parenting, will be solved when merging meshes into submeshes is implemented
        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            auto aMesh = scene->mMeshes[node->mMeshes[i]];
            auto name = aMesh->mName.C_Str();
            obj = UnityEngine::GameObject::New_ctor()->get_transform();
            obj->get_gameObject()->set_name(name);
            obj->SetParent(parentObj, false);
            obj->set_localPosition(UnityEngine::Vector3(pos.a4, pos.b4, pos.c4));

            auto renderer = obj->get_gameObject()->AddComponent<UnityEngine::SkinnedMeshRenderer*>();
            if(strstr(name, "Body"))
            {
                renderer->set_material(body.ptr());
            }

            if(strstr(name, "Face (merged).baked-0") || strstr(name, "Face (merged).baked-2"))
            {
                renderer->set_material(face.ptr());
            }

            if(strstr(name, "Face (merged).baked-1") || strstr(name, "Face (merged).baked-3"))
            {
                renderer->set_material(detail.ptr());
            }

            if(strstr(name, "Hair"))
            {
                renderer->set_material(hair.ptr());
            }

            auto mesh = loadMesh(aMesh);

            constructUnityMesh(node->mName.C_Str(), mesh, renderer);
        }
    }
    else
    {
        obj = UnityEngine::GameObject::New_ctor()->get_transform();
        obj->get_gameObject()->set_name(node->mName.C_Str());
        obj->SetParent(parentObj, false);
        obj->set_localPosition(UnityEngine::Vector3(pos.a4, pos.b4, pos.c4));
    }

    //TODO: proper solution AAAAAAAAA, requires that root bone obj is named Root plus that the meshes always come after bones
    if(strcmp(node->mName.C_Str(), "Root") == 0) 
    {
        nextBones = true;
    }

    if(nextBones)
    {
        boneObjs.push_back(obj);
    }

    //getLogger().info("%d     %s%s children: %d  locpos: %f,%f,%f,%f", test, std::string(depth, '-').c_str(), node->mName.C_Str(), node->mNumChildren, pos.a4, pos.b4, pos.c4, pos.d4);
    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        logNode(obj, node->mChildren[i], depth + 1, nextBones, scene, pos);
    }
}

void logBone(VRMC_VRM_0_0::HumanoidBone bone)
{
    getLogger().info("bone: %hhu, node idx: %d, boneObjs: %s", bone.bone, bone.node, static_cast<std::string>(boneObjs[bone.node]->get_gameObject()->get_name()).c_str());
}

#define coro(...) custom_types::Helpers::CoroutineHelper::New(__VA_ARGS__)

custom_types::Helpers::Coroutine LoadAvatar()
{
    getLogger().info("Starting Load!");

    UnityEngine::AssetBundle* ass;

    co_yield coro(VRM::ShaderLoader::LoadBundleFromFileAsync("sdcard/ModData/shaders.sbund", ass));

    if (!ass)
    {
        getLogger().error("Couldn't load bundle from file, dieing...");
        co_return;
    }

    VRMData::ShaderSO* data = nullptr;

    co_yield coro(VRM::ShaderLoader::LoadAssetFromBundleAsync(ass, "Assets/shaders.asset", reinterpret_cast<System::Type*>(csTypeOf(VRMData::ShaderSO*)), reinterpret_cast<UnityEngine::Object*&>(data)));

    if(data == nullptr)
    {
        getLogger().error("Couldn't load asset...");
        co_return;
    }

    body = UnityEngine::Object::Instantiate(data->body);
    hair = UnityEngine::Object::Instantiate(data->hair);
    face = UnityEngine::Object::Instantiate(data->face);
    detail = UnityEngine::Object::Instantiate(data->detail);

    PrewarmAllShadersOnMaterial(body.ptr());
    PrewarmAllShadersOnMaterial(hair.ptr());
    PrewarmAllShadersOnMaterial(face.ptr());
    PrewarmAllShadersOnMaterial(detail.ptr());

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile("sdcard/ModData/ava.vrm", aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_PopulateArmatureData);
    getLogger().info("Imported");
    
    auto Root = UnityEngine::GameObject::New_ctor("GLTF");
    UnityEngine::GameObject::DontDestroyOnLoad(Root);
    Root->get_transform()->set_position(UnityEngine::Vector3(0.0f, 0.0f, 0.0f));
    Root->get_transform()->set_localScale(UnityEngine::Vector3(1.0f, 1.0f, 1.0f));

    getLogger().info("Created root object");

    auto rootNode = scene->mRootNode;
    logNode(Root->get_transform(), rootNode, 0, false, scene, aiMatrix4x4());

    std::ifstream binFile("sdcard/ModData/ava.vrm", std::ios::binary);

    binFile.seekg(12); //Skip past the 12 byte header, to the json header
    uint32_t jsonLength;
    binFile.read((char*)&jsonLength, sizeof(uint32_t)); //Read the length of the json file from it's header

    std::string jsonStr;
    jsonStr.resize(jsonLength);
    binFile.seekg(20); // Skip the rest of the JSON header to the start of the string
    binFile.read(jsonStr.data(), jsonLength); // Read out the json string

    auto doc = nlohmann::json::parse(jsonStr);
    VRMC_VRM_0_0::Vrm vrm;
    VRMC_VRM_0_0::from_json(doc["extensions"]["VRM"], vrm);

    getLogger().info("parsed extensions, checking humanoid tree");

    auto humanBones = vrm.humanoid.humanBones;
    
    getLogger().info("logged bones");

    auto annotations = vrm.firstPerson.meshAnnotations;
    for (size_t i = 0; i < annotations.size(); i++)
    {
        auto x = annotations[i];
        getLogger().info("First Person: %zu   node: %d   flag: %s", i, x.mesh, x.firstPersonFlag.c_str());
    }

    getLogger().info("logged first person");

    auto avatar = VRM::Mappings::AvatarMappings::CreateAvatar(vrm, boneObjs, Root);

    getLogger().info("made avatar");

    auto anim = Root->AddComponent<UnityEngine::Animator*>();

    anim->set_avatar(avatar);

    getLogger().info("made animator");

    /*auto vrik = Root->AddComponent<RootMotion::FinalIK::VRIK*>();

    vrik->AutoDetectReferences();

    getLogger().info("made vrik");

    auto headTarget = UnityEngine::GameObject::New_ctor("HeadTarget");
    headTarget->get_transform()->set_position(UnityEngine::Vector3(1.0f, 2.0f, 4.0f));
    headTarget->get_transform()->set_rotation(UnityEngine::Quaternion::Euler(45.0f, 0.0f, 0.0f));

    vrik->solver->spine->headTarget = headTarget->get_transform();
    vrik->solver->leftArm->target = UnityEngine::GameObject::Find("ControllerLeft")->get_transform();
    vrik->solver->rightArm->target = UnityEngine::GameObject::Find("ControllerRight")->get_transform();

    vrik->Initiate();

    getLogger().info("set transforms");*/
    for(int i = 0; i < 50; i++)
    {
        auto trans = anim->GetBoneTransform(UnityEngine::HumanBodyBones(i));
        if(trans != nullptr)
        {
            getLogger().info("Bone: %s", static_cast<std::string>(trans->get_gameObject()->get_name()).c_str());
        }
    }
    
    co_return;
}
 
MAKE_HOOK_MATCH(MainMenuUIHook, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuUIHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    self->StartCoroutine(coro(LoadAvatar()));
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