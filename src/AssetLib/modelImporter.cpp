#include "AssetLib/modelImporter.hpp"

#include "MaterialTracker.hpp"
#include "config/ConfigManager.hpp"
#include "UnityEngine/AnimatorCullingMode.hpp"

#include "customTypes/AniLipSync/AnimMorphTarget.hpp"
#include "customTypes/AniLipSync/LowLatencyLipSyncContext.hpp"
#include "customTypes/BlendShape/BlendShapeController.hpp"

#include "main.hpp"
#include "paper/shared/Profiler.hpp"

SafePtrUnity<UnityEngine::Shader> AssetLib::ModelImporter::mtoon;

static Paper::Profiler VRMProfiler;

//Used for debugging armatures
void AddSphere(UnityEngine::Transform* obj)
{
    auto gameObject = UnityEngine::GameObject::CreatePrimitive(UnityEngine::PrimitiveType::Sphere)->get_transform();
    gameObject->SetParent(obj, false);
    gameObject->set_localScale(Sombrero::FastVector3(0.05f, 0.05f, 0.05f));
}

//TODO: find something better lmao
int IndexForName(const char* name, const AssetLib::Structure::ModelContext* context)
{
    for (size_t i = 0; i < context->nodes.size(); i++)
    {
        if(context->nodes[i]->name == name)
        {
            return i;
        }
    }
    return -1;
}

AssetLib::Structure::InterMeshData LoadMesh(aiMesh* mesh, AssetLib::Structure::ModelContext* context, std::optional<AssetLib::Structure::InterMeshData> existingMesh = std::nullopt)
{
    auto meshData = existingMesh.has_value() ? existingMesh.value() : AssetLib::Structure::InterMeshData();
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        auto vert = mesh->mVertices[i];
        meshData.vertices.push_back(Sombrero::FastVector3(vert.x, vert.y, vert.z));
    }
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        auto norm = mesh->mNormals[i];
        meshData.normals.push_back(Sombrero::FastVector3(norm.x, norm.y, norm.z));
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
            meshData.uv1.push_back(Sombrero::FastVector2(tex.x, tex.y));
        }
    }
    if (mesh->mTextureCoords[1] != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto tex = mesh->mTextureCoords[1][i];
            meshData.uv2.push_back(Sombrero::FastVector2(tex.x, tex.y));
        }
    }
    if (mesh->mTextureCoords[2] != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto tex = mesh->mTextureCoords[2][i];
            meshData.uv3.push_back(Sombrero::FastVector2(tex.x, tex.y));
        }
    }
    if (mesh->mTextureCoords[3] != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto tex = mesh->mTextureCoords[3][i];
            meshData.uv4.push_back(Sombrero::FastVector2(tex.x, tex.y));
        }
    }
    if (mesh->mColors[0] != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto col = mesh->mColors[0][i];
            meshData.colors.push_back(Sombrero::FastColor(col.r, col.g, col.b, col.a));
        }
    }

    meshData.vertexCounts.push_back(mesh->mNumVertices);
    meshData.materialIdxs.push_back(mesh->mMaterialIndex);

    meshData.indices.push_back(std::vector<int>(0));
    meshData.topology.push_back(UnityEngine::MeshTopology::Triangles); //TODO

    if (mesh->mFaces != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            auto face = mesh->mFaces[i];
            for (size_t x = 2; x < face.mNumIndices; x--)
            {
                meshData.indices[meshData.indices.size()-1].emplace_back(face.mIndices[x]);
            }
        }
    }

    if(context->isSkinned)
    {
        int start = 0;
        if(meshData.boneWeights.size() == 0)
        {
            meshData.boneWeights = std::vector<AssetLib::SkinTypes::BoneWeightProxy>(mesh->mNumVertices);
        }
        else
        {
            start = meshData.boneWeights.size();
            meshData.boneWeights.resize(start + mesh->mNumVertices);
        }

        //i: bone
        //j: current vertexWeight in bone
        //k: weight idx to check in vertex

        //Iterate over every bone in this mesh
        for (int i = 0; i < mesh->mNumBones; i++)
        {
            aiBone* aiBone = mesh->mBones[i];
            
            //Iterate over every vertex that this bone affects
            for (int j = 0; j < aiBone->mNumWeights; j++)
            {
                aiVertexWeight weight = aiBone->mWeights[j];

                uint VertexID = start + weight.mVertexId;

                float Weight = weight.mWeight;

                meshData.boneWeights[VertexID].AddBoneData(IndexForName(aiBone->mName.C_Str(), context), Weight);
            }
        }
    }
    for (size_t k = 0; k < mesh->mNumAnimMeshes; k++)
    {
        auto animMesh = mesh->mAnimMeshes[k];

        std::string name = animMesh->mName.C_Str();
        VRMLogger.info("blendshape {}", name.c_str());
        auto names = meshData.morphTargetNames;
        auto find = std::find(names.begin(), names.end(), name);

        int index;

        if(find == names.end())
        {
            meshData.morphTargetNames.push_back(name);
            meshData.morphTargetInfos.push_back({});
            index = meshData.morphTargetNames.size()-1;
        }
        else
        {
            index = find - names.begin();
        }

        for (size_t i = 0; i < animMesh->mNumVertices; i++)
        {
            auto vert = animMesh->mVertices[i];
            auto ogVert = mesh->mVertices[i];
            meshData.morphTargetInfos[index].vertices.push_back(Sombrero::FastVector3(vert.x - ogVert.x, vert.y - ogVert.y, vert.z - ogVert.z));
        }

        if (animMesh->mNormals != nullptr)
        {
            for (size_t i = 0; i < animMesh->mNumVertices; i++)
            {
                auto norm = animMesh->mNormals[i];
                auto ogNorm = mesh->mNormals[i];
                meshData.morphTargetInfos[index].normals.push_back(Sombrero::FastVector3(norm.x - ogNorm.x, norm.y - ogNorm.y, norm.z - ogNorm.z));
            }
        }

        if (animMesh->mTangents != nullptr)
        {
            for (size_t i = 0; i < animMesh->mNumVertices; i++)
            {
                auto tang = animMesh->mTangents[i];
                meshData.morphTargetInfos[index].tangents.push_back(Sombrero::FastVector3(tang.x, tang.y, tang.z));
            }
        }
    }

    return meshData;
}

void ConstructUnityMesh(const AssetLib::Structure::Node* node, const AssetLib::Structure::ModelContext* context)
{
    if(node->mesh.has_value())
    {
        auto mesh = node->mesh.value();
    
        UnityEngine::Mesh* unityMesh = UnityEngine::Mesh::New_ctor();
        unityMesh->set_name(node->name);
        unityMesh->set_indexFormat(mesh.vertices.size() > 65535 ? UnityEngine::Rendering::IndexFormat::UInt32 : UnityEngine::Rendering::IndexFormat::UInt16);

        unityMesh->set_vertices(il2cpp_utils::vectorToArray(mesh.vertices));
        unityMesh->set_normals(il2cpp_utils::vectorToArray(mesh.normals));
        unityMesh->set_tangents(il2cpp_utils::vectorToArray(mesh.tangents));
        unityMesh->set_uv(il2cpp_utils::vectorToArray(mesh.uv1));
        unityMesh->set_uv2(il2cpp_utils::vectorToArray(mesh.uv2));
        unityMesh->set_uv3(il2cpp_utils::vectorToArray(mesh.uv3));
        unityMesh->set_uv4(il2cpp_utils::vectorToArray(mesh.uv4));
        unityMesh->set_colors(il2cpp_utils::vectorToArray(mesh.colors));
        
        std::vector<UnityEngine::BoneWeight> convertedBW = std::vector<UnityEngine::BoneWeight>(mesh.boneWeights.size());

        for (size_t i = 0; i < mesh.boneWeights.size(); i++)
        {
            convertedBW[i] = mesh.boneWeights[i].convert();
        }
        
        unityMesh->set_boneWeights(il2cpp_utils::vectorToArray(convertedBW));
        unityMesh->set_subMeshCount(mesh.indices.size());
        uint baseVertex = 0;
        for (int i = 0; i < mesh.indices.size(); i++)
        {
            const ArrayW<int> indices = il2cpp_utils::vectorToArray<int>(mesh.indices[i]);
            unityMesh->SetIndices(indices, mesh.topology[i], i, false, static_cast<int>(baseVertex));
            baseVertex += mesh.vertexCounts[i];
        }
        unityMesh->RecalculateBounds();

        for (int i = 0; i < mesh.morphTargetNames.size(); ++i)
        {
            auto name = mesh.morphTargetNames[i];
            auto [vertices, normals, tangents] = mesh.morphTargetInfos[i];
            if(unityMesh->GetBlendShapeIndex(name) == -1 && !System::String::IsNullOrWhiteSpace(name))
            {
                unityMesh->AddBlendShapeFrame(name, 100,
                    il2cpp_utils::vectorToArray(vertices),
                    il2cpp_utils::vectorToArray(normals),
                    nullptr
                );
            }
        }

        if(context->isSkinned)
        {
            const auto renderer = node->gameObject->AddComponent<UnityEngine::SkinnedMeshRenderer*>();
            renderer->set_updateWhenOffscreen(true);
            renderer->set_allowOcclusionWhenDynamic(false);
            const auto armature = context->armature.value();

            std::vector<UnityEngine::Matrix4x4> bindPoses = std::vector<UnityEngine::Matrix4x4>(context->nodes.size());

            for (size_t i = 0; i < context->nodes.size(); i++)
            {
                const auto ctxNode = context->nodes[i];
                if(ctxNode->isBone)
                {
                    bindPoses[i] = UnityEngine::Matrix4x4::op_Multiply(ctxNode->gameObject->get_transform()->get_worldToLocalMatrix(), renderer->get_transform()->get_localToWorldMatrix());
                }
            } 

            unityMesh->set_bindposes(il2cpp_utils::vectorToArray(bindPoses));

            renderer->set_rootBone(armature.rootBone->gameObject->get_transform());

            auto bones = ArrayUtils::Select<UnityEngine::Transform*>(context->nodes,
                                    [](const AssetLib::Structure::Node* libNode){
                                        return libNode->gameObject->get_transform();
                                    }
                                );
            renderer->set_bones(il2cpp_utils::vectorToArray(bones));
            renderer->set_sharedMesh(unityMesh);
            unityMesh->RecalculateBounds();
        }
        else
        {
            const auto filter = node->gameObject->AddComponent<UnityEngine::MeshFilter*>();
            node->gameObject->AddComponent<UnityEngine::MeshRenderer*>();
            filter->set_sharedMesh(unityMesh);
        }
    }
}

Sombrero::FastVector3 convertVector(const VRMC_VRM_0_0::Vector3 vec)
{
    return Sombrero::FastVector3(vec.x, vec.y, vec.z);
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

int test = 0;
void logTransform(UnityEngine::Transform* trans, const int depth = 1)
{
    test++;
    VRMLogger.info("{} {}{}", test, std::string(depth, '-').c_str(), static_cast<std::string>(trans->get_gameObject()->get_name()).c_str());
    for (size_t i = 0; i < trans->get_childCount(); i++)
    {
        logTransform(trans->GetChild(i), depth + 1);
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
        
AssetLib::Structure::ModelContext* AssetLib::ModelImporter::Load(const std::string& filename, bool loadMaterials)
{
    const auto modelContext = new Structure::ModelContext();

    modelContext->fileName = filename;
    
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_PopulateArmatureData | aiProcess_MakeLeftHanded);
    VRMProfiler.mark("Assimp import finished");
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
                    node->mesh = LoadMesh(scene->mMeshes[node->originalNode->mMeshes[x]], modelContext);
                }
                else
                {
                    node->mesh = LoadMesh(scene->mMeshes[node->originalNode->mMeshes[x]], modelContext, node->mesh);
                }
            }
            ConstructUnityMesh(node, modelContext);
        }
    }

    VRMProfiler.mark("Constructed Meshes");

    return modelContext;
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

//TODO: Figure out 1.0.0 support
AssetLib::Structure::VRM::VRMModelContext* AssetLib::ModelImporter::LoadVRM(const std::string& filename, UnityEngine::Shader* mtoon)
{
    VRMLogger.info("Loading VRM Model: {}", filename.c_str());
    VRMProfiler.startTimer();
    VRMProfiler.mark("Start");
    const auto originalContext = Load(filename, false);
    VRMProfiler.mark("Finished initial Load");

    //Load inital data to post process
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
        textures.push_back(VRMQavatars::gLTFImageReader::ReadImageIndex(jsonLength, binFile, i));
    }
    VRMProfiler.mark("Generated Textures");

    //Generate VRM Materials

    std::vector<UnityEngine::Material*> materials;

    VRMQavatars::MaterialTracker::materials.clear();
    if(vrm.has_value())
    {
        for (size_t i = 0; i < vrm.value().materialProperties.size(); i++)
        {
            auto [name, shader, renderQueue, floatProperties, vectorProperties, textureProperties, keywordMap, tagMap] = vrm.value().materialProperties[i];

            auto mat = UnityEngine::Material::New_ctor(mtoon);

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
    }
    else
    {
        if(doc.contains("materials"))
        {
            auto vrmMaterials = doc["materials"];
            for (size_t i = 0; i < vrmMaterials.size(); i++)
            {
                VRMC_VRM_1_0::MaterialsMtoon vrmMat;
                VRMC_VRM_1_0::from_json(vrmMaterials[i], vrmMat);

                auto mat = UnityEngine::Material::New_ctor(mtoon);

                //mat->SetInt(vrmMat.transparentWithZWrite);

                auto shadeColor = vrmMat.shadeColorFactor;
                mat->SetColor("_ShadeColor", Sombrero::FastColor(shadeColor[0], shadeColor[1], shadeColor[2], shadeColor[3]));
                mat->SetTexture("_ShadeTexture", textures[vrmMat.shadeMultiplyTexture.index]);

                mat->SetFloat("_ShadeShift", vrmMat.shadingShiftFactor);
                mat->SetFloat("_ShadeToony", vrmMat.shadingToonyFactor);
                mat->SetFloat("_ShadeToony", vrmMat.giEqualizationFactor);

                auto outlineColor = vrmMat.outlineColorFactor;
                mat->SetColor("_OutlineColor", Sombrero::FastColor(outlineColor[0], outlineColor[1], outlineColor[2], outlineColor[3]));
                mat->SetFloat("_OutlineLightingMix", vrmMat.outlineLightingMixFactor);
                mat->SetFloat("_OutlineWidth", vrmMat.outlineWidthFactor);

                materials.push_back(mat);
                VRMQavatars::MaterialTracker::materials.push_back(mat);
            }
        }
    }
    VRMProfiler.mark("Created materials");
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
    VRMProfiler.mark("Set materials");

    //modelContext->rootGameObject->AddComponent<VRMQavatars::OVRLipSync::OVRLipSync*>();

    modelContext->blendShapeMaster = vrm.has_value() ? Structure::VRM::VRMBlendShapeMaster::LoadFromVRM0(vrm.value()) : Structure::VRM::VRMBlendShapeMaster::LoadFromVRM1(vrm1.value());;
    VRMProfiler.mark("Setup Blendshapes");
    auto avatar = vrm.has_value() ? VRM::Mappings::AvatarMappings::CreateAvatar(vrm.value(), modelContext->nodes, modelContext->armature.value().rootBone->gameObject) :  VRM::Mappings::AvatarMappings::CreateAvatar(vrm1.value(), modelContext->nodes, modelContext->armature.value().rootBone->gameObject);
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

                auto newMesh = VRMQavatars::BoneMeshUtility::CreateErasedMesh(skinnedRenderer->get_sharedMesh(), toErase);
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

    if(vrm.has_value())
    {
        auto colliders = std::vector<VRMQavatars::VRMSpringBoneColliderGroup*>();
        for (auto colliderGroup : vrm.value().secondaryAnimation.colliderGroups)
        {
            if (auto node = modelContext->nodes[colliderGroup.node+1]->gameObject)
            {
                auto vrmGroup = node->AddComponent<VRMQavatars::VRMSpringBoneColliderGroup*>();
                for (auto colliderRef : colliderGroup.colliders)
                {
                    vrmGroup->colliders.push_back(SphereCollider(convertVector(colliderRef.offset), colliderRef.radius));
                }
                colliders.push_back(vrmGroup);
            }
            else
            {
                break;
            }
        }

        auto springs = vrm.value().secondaryAnimation.boneGroups;
        for (size_t i = 0; i < springs.size(); i++)
        {
            auto springRef = springs[i];

            auto springBone = secondary->get_gameObject()->AddComponent<VRMQavatars::VRMSpringBone*>();

            if (springRef.center != -1)
            {
                springBone->center = modelContext->nodes[springRef.center+1]->gameObject->get_transform();
            }

            springBone->comment = springRef.comment;
            springBone->dragForce = springRef.dragForce;
            springBone->gravityDir = convertVector(springRef.gravityDir);
            springBone->gravityPower = springRef.gravityPower;
            springBone->hitRadius = springRef.hitRadius;
            springBone->stiffnessForce = springRef.stiffiness;
            if ( springRef.colliderGroups.size() > 0)
            {
                springBone->colliderGroups = std::vector<VRMQavatars::VRMSpringBoneColliderGroup*>(springRef.colliderGroups.size());
                for (int j = 0; j < springRef.colliderGroups.size(); j++)
                {
                    auto group = springRef.colliderGroups[j];
                    springBone->colliderGroups[j] = colliders[group];
                }
            }
            ArrayW<UnityEngine::Transform*> list2 = ArrayW<UnityEngine::Transform*>(springRef.bones.size());
            for (size_t k = 0; k < springRef.bones.size(); k++)
            {
                auto bone = springRef.bones[k];
                list2[k] = modelContext->nodes[bone+1]->gameObject->get_transform();
            }
            springBone->rootBones = list2;
        }
    }
    else if(exts.contains("VRMC_springBone"))
    {
        /*auto sprigEx = exts["VRMC_springBone"];

        auto sprigColliders = sprigEx["colliders"];

        auto colliders = std::vector<SphereCollider>();
        for (auto collider : sprigColliders)
        {
            colliders.push_back(SphereCollider(convertVector(collider["offset"]), collider["radius"]));
        }

        auto colliderGroups = std::vector<VRMQavatars::VRMSpringBoneColliderGroup*>();
        for (auto colliderGroup : sprigEx["colliderGroups"])
        {
            auto firstCollider = static_cast<int>(colliderGroup["colliders"][0]);
            if (auto node = modelContext->nodes[static_cast<int>(sprigColliders[firstCollider]["node"])+1]->gameObject)
            {
                auto vrmGroup = node->AddComponent<VRMQavatars::VRMSpringBoneColliderGroup*>();
                for (auto colliderRef : colliderGroup["colliders"])
                {
                    vrmGroup->colliders.push_back(colliders[colliderRef]);
                }
                colliderGroups.push_back(vrmGroup);
            }
            else
            {
                break;
            }
        }

        auto springs = sprigEx["springs"];
        for (auto springRef : sprigEx["springs"])
        {
            auto springBone = secondary->get_gameObject()->AddComponent<VRMQavatars::VRMSpringBone*>();

            if (springRef.contains("center") && springRef["center"] != -1)
            {
                springBone->center = modelContext->nodes[static_cast<int>(springRef["center"])+1]->gameObject->get_transform();
            }

            springBone->comment = springRef["name"];
            springBone->dragForce = springRef.dragForce;
            springBone->gravityDir = convertVector(springRef.gravityDir);
            springBone->gravityPower = springRef.gravityPower;
            springBone->hitRadius = springRef.hitRadius;
            springBone->stiffnessForce = springRef.stiffiness;
            if ( springRef["colliderGroups"].size() > 0)
            {
                springBone->colliderGroups = std::vector<VRMQavatars::VRMSpringBoneColliderGroup*>(springRef["colliderGroups"].size());
                for (int j = 0; j < springRef["colliderGroups"].size(); j++)
                {
                    auto group = springRef["colliderGroups"][j];
                    springBone->colliderGroups[j] = colliderGroups[group];
                }
            }
            ArrayW<UnityEngine::Transform*> list2 = ArrayW<UnityEngine::Transform*>(springRef.bones.size());
            for (size_t k = 0; k < springRef.bones.size(); k++)
            {
                auto bone = springRef.bones[k];
                list2[k] = modelContext->nodes[bone+1]->gameObject->get_transform();
            }
            springBone->rootBones = list2;
        }*/
    }

    VRMProfiler.mark("Created Springbones");
    VRMProfiler.endTimer();
    VRMProfiler.printMarks();
    return modelContext;
}