#include "AssetLib/modelImporter.hpp"

#include <fstream>

void IterateArmatureNode(aiNode* node, AssetLib::Structure::Node* parentNodeStructure, AssetLib::Structure::ModelContext* context)
{
    auto nodeStructure = new AssetLib::Structure::Node();
    nodeStructure->isBone = true; //we are iterating over the armature so obviously it's a bone... right?
    nodeStructure->name = node->mName.C_Str();
    nodeStructure->isRoot = node->mParent == nullptr;
    nodeStructure->parent = parentNodeStructure;
    nodeStructure->position = UnityEngine::Vector3(node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4);

    context->nodes.push_back(nodeStructure);

    auto& armature = context->armature.value();
    armature.rootBone = nodeStructure->isRoot ? nodeStructure : armature.rootBone;
    armature.bones.push_back(nodeStructure);

    if(parentNodeStructure != nullptr)
    {
        parentNodeStructure->children.push_back(nodeStructure);
    }

    for (int i = 0; i < node->mNumChildren; i++)
    {
        IterateArmatureNode(node->mChildren[i], nodeStructure, context);
    }
}

void AddSphere(UnityEngine::Transform* obj)
{
    auto gameObject = UnityEngine::GameObject::CreatePrimitive(UnityEngine::PrimitiveType::Sphere)->get_transform();
    gameObject->SetParent(obj, false);
    gameObject->set_localScale(UnityEngine::Vector3(0.05f, 0.05f, 0.05f));
}

void CreateBoneStructure(AssetLib::Structure::Node* node, UnityEngine::Transform* parentTrans)
{
    auto nodeTrans = UnityEngine::GameObject::New_ctor(node->name)->get_transform();
    UnityEngine::GameObject::DontDestroyOnLoad(nodeTrans->get_gameObject());

    nodeTrans->SetParent(parentTrans, false);
    nodeTrans->set_localPosition(node->position);
    AddSphere(nodeTrans);

    for (size_t i = 0; i < node->children.size(); i++)
    {
        CreateBoneStructure(node->children[i], nodeTrans);
    }
    
}

AssetLib::Structure::ModelContext* AssetLib::ModelImporter::Load(const std::string& filename)
{
    auto modelContext = new AssetLib::Structure::ModelContext();
    
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_PopulateArmatureData);
    
    auto Root = UnityEngine::GameObject::New_ctor("Model");
    UnityEngine::GameObject::DontDestroyOnLoad(Root);
    Root->get_transform()->set_position(UnityEngine::Vector3(0.0f, 0.0f, 0.0f));
    Root->get_transform()->set_localScale(UnityEngine::Vector3(1.0f, 1.0f, 1.0f));

    aiNode* aiArmature;

    //We assume that the first skinned mesh loaded by assimp has the armature used by the rest of the model. 
    //If you need more than one then make a PR that implements that or something
    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        if(scene->mMeshes[i]->mNumBones > 0)
        {
            aiArmature = scene->mMeshes[i]->mBones[0]->mArmature;
            modelContext->isSkinned = true;
        }
    }

    getLogger().info("skinned, %d", modelContext->isSkinned);
    //If we are skinned we want to process the armature first
    if(modelContext->isSkinned)
    {
        //Get first available armature
        auto armature = aiArmature;

        modelContext->armature = AssetLib::Structure::Armature();
        IterateArmatureNode(armature, nullptr, modelContext);

        CreateBoneStructure(modelContext->armature.value().rootBone, Root->get_transform());
    }

    return modelContext;
}

//TODO: Figure out 1.0.0 support
AssetLib::Structure::VRM::VRMModelContext* AssetLib::ModelImporter::LoadVRM(const std::string& filename)
{
    //Load inital data to post process
    auto context = Load(filename);

    //Load in binary to parse out VRM data

    std::ifstream binFile(filename, std::ios::binary);

    binFile.seekg(12); //Skip past the 12 byte header, to the json header
    uint32_t jsonLength;
    binFile.read((char*)&jsonLength, sizeof(uint32_t)); //Read the length of the json file from it's header

    std::string jsonStr;
    jsonStr.resize(jsonLength);
    binFile.seekg(20); // Skip the rest of the JSON header to the start of the string
    binFile.read(jsonStr.data(), jsonLength); // Read out the json string

    auto doc = nlohmann::json::parse(jsonStr);
    VRMC_VRM_0_0::Vrm vrm; //final ext Data;
    VRMC_VRM_0_0::from_json(doc["extensions"]["VRM"], vrm);

    return nullptr;
}