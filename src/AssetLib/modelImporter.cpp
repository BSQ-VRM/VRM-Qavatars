#include "AssetLib/modelImporter.hpp"

#include "MaterialTracker.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "config/ConfigManager.hpp"
#include "UnityEngine/AnimatorCullingMode.hpp"

#include "customTypes/AniLipSync/AnimMorphTarget.hpp"
#include "customTypes/AniLipSync/LowLatencyLipSyncContext.hpp"
#include "customTypes/BlendShape/BlendShapeController.hpp"

#include "main.hpp"
#include "paper/shared/Profiler.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include <functional>
#include <thread>

#include "AssetLib/gLTFImageReader.hpp"
#include "AssetLib/generators/meshGenerator.hpp"
#include "AssetLib/generators/avatarGenerator.hpp"
#include "AssetLib/generators/intermediateMeshGenerator.hpp"
#include "AssetLib/generators/textureGenerator.hpp"
#include "AssetLib/generators/materialGenerator.hpp"

#include "AssetLib/boneMeshUtility.hpp"

#include "json.hpp"

SafePtrUnity<UnityEngine::Shader> AssetLib::ModelImporter::mtoon;

static Paper::Profiler VRMProfiler;

namespace AssetLib
{
    static Assimp::Importer* importer;
    void LoadScene(std::string const& fileName, std::function<void(aiScene const*)> const& onFinished) {
        std::thread([](const std::string& fileName, const std::function<void(aiScene const*)>& onFinished){
            static auto importer = Assimp::Importer();
            auto scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_PopulateArmatureData | aiProcess_MakeLeftHanded);
            if (!scene) {
                onFinished(nullptr);
                return;
            }
            
            BSML::MainThreadScheduler::Schedule(std::bind(onFinished, scene));
        }, fileName, onFinished).detach();
    }

    bool anyChildMeshesAssimp(aiNode* node)
    {
        bool anymesh = false;
        for (size_t i = 0; i < node->mNumChildren; i++)
        {
            anymesh |= node->mChildren[i]->mNumMeshes > 0;
        }
        return anymesh;
    }

    void IterateNode(aiNode* assnode, AssetLib::Structure::Node* parentNode, AssetLib::Structure::ModelContext* context)
    {
        const auto node = new AssetLib::Structure::Node(std::string(assnode->mName.C_Str()), {}, parentNode, false, false, Sombrero::FastVector3(assnode->mTransformation.a4, assnode->mTransformation.b4, assnode->mTransformation.c4), std::nullopt, assnode);

        if(assnode->mNumMeshes < 1 && !anyChildMeshesAssimp(assnode))
        {
            node->isBone = true;
            context->armature.value().bones.push_back(node);
        }

        if(parentNode == nullptr)
        {
            context->rootNode = node;
        }
        else
        {
            parentNode->children.push_back(node);
        }

        context->nodes.push_back(node);
        for (size_t i = 0; i < assnode->mNumChildren; i++)
        {
            IterateNode(assnode->mChildren[i], node, context);
        }
    }

    void CreateNodeTreeObject(AssetLib::Structure::Node* node)
    {
        if(node->gameObject == nullptr)
        {
            node->gameObject = UnityEngine::GameObject::New_ctor(node->name);
            if(node->parent != nullptr && node->parent->gameObject != nullptr)
            {
                auto trans = node->gameObject->get_transform();
                trans->SetParent(node->parent->gameObject->get_transform());
            }
            node->gameObject->get_transform()->set_localPosition(node->position);
            node->processed = true;
            //AddSphere(node->gameObject->get_transform());
        }
        for (size_t i = 0; i < node->children.size(); i++)
        {
            CreateNodeTreeObject(node->children[i]);
        }
    }

    bool anyChildMeshes(AssetLib::Structure::Node* node)
    {
        bool anymesh = false;
        for (size_t i = 0; i < node->children.size(); i++)
        {
            anymesh |= !node->isBone;
        }
        return anymesh;
    }

    void logAssimpNode(aiNode* node, int depth)
    {
        VRMLogger.info("{}{}", std::string(depth, '-'), node->mName.C_Str());
        for (size_t i = 0; i < node->mNumChildren; i++)
        {
            logAssimpNode(node->mChildren[i], depth + 1);
        }
    }

    void ModelImporter::Load(const std::string& filename, bool loadMaterials, const std::function<void(Structure::ModelContext*)>& onFinish)
    {
        LoadScene(filename, [&onFinish, filename](const aiScene* scene) {
            if (!scene)
            {
                onFinish(nullptr);
                return;
            }

            logAssimpNode(scene->mRootNode, 0);

            const auto modelContext = new Structure::ModelContext();
            modelContext->fileName = filename;
            modelContext->originalScene = scene;

            const auto Root = UnityEngine::GameObject::New_ctor("ROOT");
            UnityEngine::GameObject::DontDestroyOnLoad(Root);
            Root->get_transform()->set_position(Sombrero::FastVector3(0.0f, 0.0f, 0.0f));
            Root->get_transform()->set_rotation(UnityEngine::Quaternion::Euler(0.0f, 0.0f, 0.0f));
            Root->get_transform()->set_localScale(Sombrero::FastVector3(1.0f, 1.0f, 1.0f));
            modelContext->rootGameObject = Root;

            modelContext->armature = Structure::Armature();

            VRMProfiler.mark("Creating Node Tree");

            //STEP ONE: Create initial node tree

            IterateNode(scene->mRootNode, nullptr, modelContext);
            modelContext->rootNode->gameObject = Root;
            modelContext->rootNode->processed = true;

            VRMProfiler.mark("Node Tree Created");

            //STEP TWO: Create gameobjects for each node

            CreateNodeTreeObject(modelContext->rootNode);

            VRMProfiler.mark("Created GameObjects");

            //STEP THREE: Load in armature

            Structure::Node* armatureNode = nullptr;

            //TODO: perform this how the assimp docs say to (i'm lazy)
            for (size_t i = 1; i < modelContext->nodes.size(); i++)
            {
                const auto node = modelContext->nodes[i];
                if(node->children.size() > 0 && !anyChildMeshes(node))
                {
                    modelContext->isSkinned = true;
                    armatureNode = node;
                    break;
                }
            }
            modelContext->armature.value().rootBone = armatureNode;

            VRMProfiler.mark("Loaded Armature");

            //STEP FOUR: Load in meshes

            auto intermediateMeshGenerator = Generators::IntermediateMeshGenerator();
            auto meshGenerator = Generators::MeshGenerator();

            for (size_t i = 0; i < modelContext->nodes.size(); i++)
            {
                const auto node = modelContext->nodes[i];
                if(node->originalNode->mNumMeshes > 0)
                {
                    node->isBone = false;
                    for (size_t x = 0; x < node->originalNode->mNumMeshes; x++)
                    {
                        if(x == 0)
                        {
                            node->mesh = intermediateMeshGenerator.Generate(scene->mMeshes[node->originalNode->mMeshes[x]], modelContext);
                        }
                        else
                        {
                            node->mesh = intermediateMeshGenerator.Generate(scene->mMeshes[node->originalNode->mMeshes[x]], modelContext, node->mesh);
                        }
                    }
                }
            }

            VRMProfiler.mark("Constructed Intermediate Meshes");

            //STEP FIVE: Generate Unity meshes

            for (size_t i = 0; i < modelContext->nodes.size(); i++)
            {
                const auto node = modelContext->nodes[i];
                if(node->originalNode->mNumMeshes > 0)
                {
                    meshGenerator.Generate(node, modelContext);
                }
            }

            VRMProfiler.mark("Generated Unity Meshes");

            onFinish(modelContext);
        });
    }

    std::vector<UnityEngine::Transform*> Ancestors(UnityEngine::Transform* root)
    {
        std::vector<UnityEngine::Transform*> ancestors;

        UnityEngine::Transform* currentTrans = root;

        while(currentTrans != nullptr)
        {
            ancestors.push_back(currentTrans);
            currentTrans = currentTrans->get_parent();
        }

        return ancestors;
    }

    void SetXLocalRot(UnityEngine::Transform* trans, const float x)
    {
        const auto rot = trans->get_localRotation().get_eulerAngles();
        trans->set_localRotation(UnityEngine::Quaternion::Euler(x, rot.y, rot.z));
    }

    void ModelImporter::LoadVRM(const std::string& filename, const std::function<void(Structure::VRM::VRMModelContext*)>& onFinish)
    {
        //Don't load materials because we are replacing them with VRM materials
        Load(filename, false, [onFinish, filename](Structure::ModelContext* originalContext){
            if (!originalContext)
            {
                onFinish(nullptr);
                return;
            }

            VRMProfiler.printMarks();

            VRMProfiler.mark("Finished initial Load");

            const auto modelContext = new Structure::VRM::VRMModelContext(std::move(*originalContext));//Don't load materials because we are replacing them with VRM materials

            //Load in binary to parse out VRM data

            std::ifstream binFile(filename, std::ios::binary);

            binFile.seekg(12); //Skip past the 12 byte header, to the json header
            uint32_t jsonLength;
            binFile.read(reinterpret_cast<char*>(&jsonLength), sizeof(uint32_t)); //Read the length of the json file from it's header

            std::string jsonStr;
            jsonStr.resize(jsonLength);
            binFile.seekg(20); // Skip the rest of the JSON header to the start of the string
            binFile.read(jsonStr.data(), jsonLength); // Read out the json string

            auto doc = nlohmann::json::parse(jsonStr);
            std::optional<VRMC_VRM_0_0::Vrm> vrm; //final ext Data;
            std::optional<VRMC_VRM_1_0::Vrm> vrm1; //final ext Data;

            auto exts = doc["extensions"];
            if(exts.contains("VRM"))
            {
                VRMC_VRM_0_0::Vrm vrm0;
                from_json(exts["VRM"], vrm0);
                vrm = vrm0;
                modelContext->vrm0 = vrm;
            }

            if(exts.contains("VRMC_vrm"))
            {
                VRMC_VRM_1_0::Vrm vrm10;
                from_json(exts["VRMC_vrm"], vrm10);
                vrm1 = vrm10;
                modelContext->vrm1 = vrm1;
            }

            VRMProfiler.mark("Parsed VRM data");

            //Generate Textures

            std::vector<UnityEngine::Texture2D*> textures;
            for (size_t i = 0; i < doc["images"].size(); i++)
            {
                textures.push_back(AssetLib::gLTFImageReader::ReadImageIndex(jsonLength, binFile, i));
            }

            //Generate VRM Materials

            std::vector<UnityEngine::Material*> materials;

            VRMQavatars::MaterialTracker::materials.clear();
            for (size_t i = 0; i < vrm.value().materialProperties.size(); i++)
            {
                auto [name, shader, renderQueue, floatProperties, vectorProperties, textureProperties, keywordMap, tagMap] = vrm.value().materialProperties[i];
                
                auto mat = UnityEngine::Material::New_ctor(ModelImporter::mtoon.ptr());

                for (const auto & [ key, value ] : floatProperties )
                {
                    mat->SetFloat(key.c_str(), value);
                }

                for (const auto & [ key, value ] : keywordMap )
                {
                    if(value)
                    {
                        mat->EnableKeyword(key.c_str());
                    }
                    else
                    {
                        mat->DisableKeyword(key.c_str());
                    }

                }

                for (const auto & [ key, value ] : textureProperties )
                {
                    mat->SetTexture(key.c_str(), textures[value]);
                }

                for (const auto & [ key, value ] : vectorProperties )
                {
                    mat->SetColor(key.c_str(), Sombrero::FastColor(value[0], value[1], value[2], value[3]));
                }

                for (const auto & [ key, value ] : tagMap )
                {
                    mat->SetOverrideTag(key, value);
                }

                mat->set_renderQueue(renderQueue);

                materials.push_back(mat);
                VRMQavatars::MaterialTracker::materials.push_back(mat);
            }
            VRMQavatars::MaterialTracker::UpdateMaterials();
            for (size_t i = 0; i < modelContext->nodes.size(); i++)
            {
                if(auto node = modelContext->nodes[i]; node->mesh.has_value() && node->processed)
                {
                    auto mesh = node->mesh.value();
                    ArrayW<UnityEngine::Material*> matArray = ArrayW<UnityEngine::Material*>(mesh.materialIdxs.size());
                    for (size_t k = 0; k < mesh.materialIdxs.size(); k++)
                    {
                        matArray[k] = materials[mesh.materialIdxs[k]];
                    }
                    
                    auto renderer = node->gameObject->GetComponent<UnityEngine::SkinnedMeshRenderer*>();
                    renderer->set_sharedMaterials(matArray);
                }
            }

            //modelContext->rootGameObject->AddComponent<VRMQavatars::OVRLipSync::OVRLipSync*>();

            modelContext->blendShapeMaster = vrm.has_value() ? Structure::VRM::VRMBlendShapeMaster::LoadFromVRM0(vrm.value()) : Structure::VRM::VRMBlendShapeMaster::LoadFromVRM1(vrm1.value());

            VRMProfiler.mark("Setup Blendshapes");

            auto avatarGenerator = Generators::AvatarGenerator();
            auto avatar = vrm.has_value() ? avatarGenerator.Generate(vrm.value(), modelContext->nodes, modelContext->armature.value().rootBone->gameObject) : avatarGenerator.Generate(vrm1.value(), modelContext->nodes, modelContext->armature.value().rootBone->gameObject);
            
            VRMProfiler.mark("Setup Avatar Object");
            
            auto anim = modelContext->rootGameObject->AddComponent<UnityEngine::Animator*>();
            anim->set_cullingMode(UnityEngine::AnimatorCullingMode::AlwaysAnimate);
            anim->set_avatar(avatar);

            //Fix crossed legs

            auto LUleg = anim->GetBoneTransform(UnityEngine::HumanBodyBones::LeftUpperLeg);
            auto RUleg = anim->GetBoneTransform(UnityEngine::HumanBodyBones::RightUpperLeg);
            auto LLleg = anim->GetBoneTransform(UnityEngine::HumanBodyBones::LeftLowerLeg);
            auto RLleg = anim->GetBoneTransform(UnityEngine::HumanBodyBones::RightLowerLeg);

            SetXLocalRot(LUleg, -4.0f);
            SetXLocalRot(RUleg, -4.0f);
            SetXLocalRot(LLleg, 4.0f);
            SetXLocalRot(RLleg, 4.0f);

            modelContext->rootGameObject->AddComponent<VRMQavatars::BlendShape::BlendShapeController*>();

            auto vrik = modelContext->rootGameObject->AddComponent<VRMQavatars::FinalIK::VRIK*>();

            auto targetManager = modelContext->rootGameObject->AddComponent<VRMQavatars::TargetManager*>();
            targetManager->vrik = vrik;
            targetManager->Initialize();

            VRMProfiler.mark("Target Manager Initialized");

            auto headBone = anim->GetBoneTransform(UnityEngine::HumanBodyBones::Neck);
            VRMLogger.info("headBone {}", static_cast<std::string>(headBone->get_gameObject()->get_name()).c_str());
            for (size_t i = 0; i < modelContext->nodes.size(); i++)
            {
                if(const auto node = modelContext->nodes[i]; node->mesh.has_value() && node->processed)
                {
                    auto gameObject = modelContext->nodes[i]->gameObject;
                    if(!VRMQavatars::Config::ConfigManager::GetGlobalConfig().ForceHideBody.GetValue())
                    {
                        auto skinnedRenderer = gameObject->GetComponent<UnityEngine::SkinnedMeshRenderer*>();

                        std::vector<int> toErase;

                        for (size_t j = 0; j < modelContext->nodes.size(); j++)
                        {
                            auto bone = modelContext->nodes[j];
                            if(bone->isBone)
                            {
                                if(auto ancestors = Ancestors(bone->gameObject->get_transform()); std::find(ancestors.begin(), ancestors.end(), headBone.ptr()) != ancestors.end())
                                {
                                    toErase.push_back(j);
                                }
                            }
                        }

                        auto newMesh = AssetLib::BoneMeshUtility::CreateErasedMesh(skinnedRenderer->get_sharedMesh(), toErase);
                        auto newObj = UnityEngine::GameObject::Instantiate(gameObject, gameObject->get_transform(), false);
                        newObj->GetComponent<UnityEngine::SkinnedMeshRenderer*>()->set_sharedMesh(newMesh);

                        //SetLayers
                        newObj->set_layer(6); //First Person
                    }
                    if(!VRMQavatars::Config::ConfigManager::GetGlobalConfig().ForceHead.GetValue() || VRMQavatars::Config::ConfigManager::GetGlobalConfig().ForceHideBody.GetValue())
                    {
                        gameObject->set_layer(3); //Third Person
                    }
                }
            }

            VRMProfiler.mark("Seperated First & Third Person Meshes");

            auto secondary = UnityEngine::GameObject::New_ctor("Secondary");
            secondary->get_transform()->SetParent(modelContext->rootGameObject->get_transform());

            //modelContext->rootGameObject->AddComponent<VRMQavatars::AniLipSync::LowLatencyLipSyncContext*>();
            //modelContext->rootGameObject->AddComponent<VRMQavatars::AniLipSync::AnimMorphTarget*>();

            VRMProfiler.mark("Created Springbones");
            VRMProfiler.endTimer();
            VRMProfiler.printMarks();

            onFinish(modelContext);
        });
    }
}