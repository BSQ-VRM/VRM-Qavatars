#include "AssetLib/modelImporter.hpp"

//Loads the armature into the model context
void IterateArmatureNode(aiNode* node, AssetLib::Structure::Node* parentNodeStructure, AssetLib::Structure::ModelContext* context)
{
    auto nodeStructure = new AssetLib::Structure::Node();
    nodeStructure->isBone = true; //we are iterating over the armature so obviously it's a bone... right?
    nodeStructure->name = node->mName.C_Str();
    nodeStructure->isRoot = parentNodeStructure == nullptr;
    nodeStructure->parent = parentNodeStructure;
    nodeStructure->position = UnityEngine::Vector3(node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4);

    context->nodes.push_back(nodeStructure);

    auto& armature = context->armature.value();
    armature.rootBone = nodeStructure->isRoot ? nodeStructure : armature.rootBone;
    armature.bones.push_back(nodeStructure);

    if(nodeStructure->isRoot)
    {
        context->rootNode = nodeStructure;
    }

    if(parentNodeStructure != nullptr)
    {
        parentNodeStructure->children.push_back(nodeStructure);
    }

    for (int i = 0; i < node->mNumChildren; i++)
    {
        IterateArmatureNode(node->mChildren[i], nodeStructure, context);
    }
}

//Used for debugging armatures
void AddSphere(UnityEngine::Transform* obj)
{
    auto gameObject = UnityEngine::GameObject::CreatePrimitive(UnityEngine::PrimitiveType::Sphere)->get_transform();
    gameObject->SetParent(obj, false);
    gameObject->set_localScale(UnityEngine::Vector3(0.05f, 0.05f, 0.05f));
}

//Creates the bone structure in the scene under the model context root
void CreateBoneStructure(AssetLib::Structure::Node* node, UnityEngine::Transform* parentTrans)
{
    auto nodeTrans = UnityEngine::GameObject::New_ctor(node->name)->get_transform();
    UnityEngine::GameObject::DontDestroyOnLoad(nodeTrans->get_gameObject());

    nodeTrans->SetParent(parentTrans, false);
    nodeTrans->set_localPosition(node->position);
    AddSphere(nodeTrans);

    node->gameObject = nodeTrans->get_gameObject();
    node->processed = true;

    for (size_t i = 0; i < node->children.size(); i++)
    {
        CreateBoneStructure(node->children[i], nodeTrans);
    }
    
}

//TODO: find something better lmao
int IndexForName(const char* name, AssetLib::Structure::ModelContext* context)
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

void LoadMesh(aiMesh* mesh, AssetLib::Structure::ModelContext* context)
{
    auto meshData = AssetLib::Structure::InterMeshData();
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

    meshData.materialIdx = mesh->mMaterialIndex;

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
    if(context->isSkinned)
    {
        meshData.boneWeights = std::vector<AssetLib::SkinTypes::BoneWeightProxy>(mesh->mNumVertices);

        //i: bone
        //j: current vertexWeight in bone
        //k: weight idx to check in vertex

        //Iterate over every bone in this mesh
        for (int i = 0; i < mesh->mNumBones; i++)
        {
            aiBone* aiBone = mesh->mBones[i];
            getLogger().info("Bone %d: %s", i, aiBone->mName.C_Str());
            
            //Iterate over every vertex that this bone affects
            for (int j = 0; j < aiBone->mNumWeights; j++)
            {
                aiVertexWeight weight = aiBone->mWeights[j];

                                 //save for submeshing?
                uint VertexID = /*m_Entries[MeshIndex].BaseVertex*/ + weight.mVertexId;

                float Weight = weight.mWeight;

                meshData.boneWeights[VertexID].AddBoneData(IndexForName(aiBone->mName.C_Str(), context), Weight);
            }
        }
    }

    //TODO: Do we need to sort vertex weights? Or does assimp do this for us.

    //TODO: Load in animations/blendshapes. Do we do this now or as a postprocess step?

    auto node = new AssetLib::Structure::Node(mesh->mName.C_Str(), {}, context->nodes.front(), false, false, UnityEngine::Vector3::get_zero(), meshData);

    context->nodes.push_back(node);
    context->rootNode->children.push_back(node);
}

void ConstructUnityMesh(AssetLib::Structure::Node* node, UnityEngine::Transform* rootTransform, AssetLib::Structure::ModelContext* context)
{
    if(node->mesh.has_value())
    {
        auto mesh = node->mesh.value();
        getLogger().info("constructing renderer %s", node->name.c_str());

        auto nodeTrans = UnityEngine::GameObject::New_ctor(node->name)->get_transform();
        nodeTrans->SetParent(rootTransform, false);
        nodeTrans->set_localPosition(node->position);
        node->gameObject = nodeTrans->get_gameObject();
        node->processed = true;
    
        UnityEngine::Mesh* unityMesh = UnityEngine::Mesh::New_ctor();
        unityMesh->set_name(node->name);
        unityMesh->set_indexFormat(mesh.vertices.size() > 65535 ? UnityEngine::Rendering::IndexFormat::UInt32 : UnityEngine::Rendering::IndexFormat::UInt16);
        
        getLogger().info("Mesh object created, setting values");

        unityMesh->set_vertices(ArrayUtils::vector2ArrayW(mesh.vertices));
        unityMesh->set_normals(ArrayUtils::vector2ArrayW(mesh.normals));
        unityMesh->set_tangents(ArrayUtils::vector2ArrayW(mesh.tangents));
        unityMesh->set_uv(ArrayUtils::vector2ArrayW(mesh.uv1));
        unityMesh->set_uv2(ArrayUtils::vector2ArrayW(mesh.uv2));
        unityMesh->set_uv3(ArrayUtils::vector2ArrayW(mesh.uv3));
        unityMesh->set_uv4(ArrayUtils::vector2ArrayW(mesh.uv4));
        unityMesh->set_colors(ArrayUtils::vector2ArrayW(mesh.colors));

        getLogger().info("loading bones");
        
        std::vector<UnityEngine::BoneWeight> convertedBW = std::vector<UnityEngine::BoneWeight>(mesh.boneWeights.size());

        for (size_t i = 0; i < mesh.boneWeights.size(); i++)
        {
            convertedBW[i] = mesh.boneWeights[i].convert();
            float weightSum = convertedBW[i].m_Weight0 + convertedBW[i].m_Weight1 + convertedBW[i].m_Weight2 + convertedBW[i].m_Weight3;
        }

        getLogger().info("loaded bones");
        
        unityMesh->set_boneWeights(ArrayUtils::vector2ArrayW(convertedBW));

        unityMesh->set_subMeshCount(mesh.indices.size());
        uint baseVertex = 0;
        for (int i = 0; i < mesh.indices.size(); i++)
        {
            unityMesh->SetIndices(ArrayW<int>(ArrayUtils::vector2ArrayW(mesh.indices[i])), mesh.topology[i], i, false, (int)baseVertex);
            //baseVertex += getVertexCount(mesh.primitives[i], model);
        }
        unityMesh->RecalculateBounds();

        //Support blendshapes one day
        /*if (mesh.morphTargetVertices.size() > 0)
        {
            for (int i = 0; i < mesh.morphTargetVertices.size(); i++)
            {
                auto targetName = "Morphtarget " + std::to_string(i);
                unityMesh->AddBlendShapeFrame(targetName, 100,
                    ArrayUtils::vector2ArrayW(mesh.morphTargetVertices[i]),
                    mesh.morphTargetNormals.size() > 0 ? ArrayUtils::vector2ArrayW(mesh.morphTargetNormals[i]) : nullptr,
                    mesh.morphTargetTangents.size() > 0 ? ArrayUtils::vector2ArrayW(mesh.morphTargetTangents[i]) : nullptr
                );
            }
        }*/

        if (mesh.normals.size() < 1 && mesh.topology[0] == UnityEngine::MeshTopology::Triangles)
        {
            unityMesh->RecalculateNormals();
        }

        if(context->isSkinned)
        {
            auto renderer = nodeTrans->get_gameObject()->AddComponent<UnityEngine::SkinnedMeshRenderer*>();

            auto armature = context->armature.value();

            std::vector<UnityEngine::Matrix4x4> bindPoses = std::vector<UnityEngine::Matrix4x4>(armature.bones.size());

            for (size_t i = 0; i < armature.bones.size(); i++)
            {
                bindPoses[i] = armature.bones[i]->gameObject->get_transform()->get_worldToLocalMatrix() * renderer->get_transform()->get_localToWorldMatrix();
            }

            getLogger().info("%lu", convertedBW.size());
            getLogger().info("%lu", armature.bones.size());
            getLogger().info("%lu", bindPoses.size());
            getLogger().info("%s", armature.rootBone->name.c_str());

            unityMesh->set_bindposes(ArrayUtils::vector2ArrayW(bindPoses));

            renderer->set_rootBone(armature.rootBone->gameObject->get_transform());
            renderer->set_bones(
                ArrayUtils::vector2ArrayW(
                    ArrayUtils::Select<UnityEngine::Transform*>(armature.bones, 
                        [](AssetLib::Structure::Node* node){
                            return node->gameObject->get_transform();
                        }
                    )
                )
            );

            renderer->set_sharedMesh(unityMesh);
        }
        else
        {
            auto filter = nodeTrans->get_gameObject()->AddComponent<UnityEngine::MeshFilter*>();
            auto renderer = nodeTrans->get_gameObject()->AddComponent<UnityEngine::MeshRenderer*>();

            filter->set_sharedMesh(unityMesh);
        }

        
        getLogger().info("mesh components setup");
    }
    else
    {
        //die
        return;
    }
}

AssetLib::Structure::ModelContext* AssetLib::ModelImporter::Load(const std::string& filename, bool loadMaterials)
{
    auto modelContext = new AssetLib::Structure::ModelContext();
    
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_PopulateArmatureData);
    modelContext->originalScene = scene;
    
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
            getLogger().info("Armature %d: %d", i, scene->mMeshes[i]->mNumBones);
        }
    }

    getLogger().info("skinned, %d", modelContext->isSkinned);
    //If we are skinned we want to process the armature first
    if(modelContext->isSkinned)
    {
        //Get first available armature
        auto armature = aiArmature;

        modelContext->armature = AssetLib::Structure::Armature();
        IterateArmatureNode(armature->mChildren[0], nullptr, modelContext);

        CreateBoneStructure(modelContext->armature.value().rootBone, Root->get_transform());
    }

    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
        LoadMesh(scene->mMeshes[i], modelContext);
    }
    

    for (size_t i = 0; i < modelContext->rootNode->children.size(); i++)
    {
        auto node = modelContext->rootNode->children[i];
        if(node->mesh.has_value())
        {
            ConstructUnityMesh(node, Root->get_transform(), modelContext);
        }
    }

    return modelContext;
}

ArrayW<uint8_t> ReadImage(nlohmann::json bufferView, uint32_t jsonLength, std::ifstream& binFile, int x)
{
    uint32_t size = bufferView["byteLength"].get<uint32_t>();
    uint32_t start = bufferView["byteOffset"].get<uint32_t>();
    std::string thing;
    thing.resize(size);
    binFile.seekg((28 + jsonLength) + start);
    binFile.read(thing.data(), size);

    FILE* p;
    p = fopen(("sdcard/ModData/vrm_" + std::to_string(x) + ".png").c_str(), "wb");
    fwrite(thing.data(), size, 1, p);
    ::fclose(p);

    return ArrayW<uint8_t>(reinterpret_cast<uint8_t*>(thing.data()));
}

UnityEngine::Texture2D* FileToSprite(std::string filePath)
{
    std::ifstream instream(filePath, std::ios::in | std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
    Array<uint8_t>* bytes = il2cpp_utils::vectorToArray(data);
    UnityEngine::Texture2D* texture = UnityEngine::Texture2D::New_ctor(0, 0, UnityEngine::TextureFormat::RGBA32, false, false);
    if (UnityEngine::ImageConversion::LoadImage(texture, bytes, false))
        return texture;
    return nullptr;
}

//TODO: Figure out 1.0.0 support
AssetLib::Structure::VRM::VRMModelContext* AssetLib::ModelImporter::LoadVRM(const std::string& filename, UnityEngine::Shader* mtoon)
{
    getLogger().info("x1");
    const auto originalContext = Load(filename, false);

    getLogger().info("x2");
    //Load inital data to post process
    const auto modelContext = new AssetLib::Structure::VRM::VRMModelContext(originalContext);//Don't load materials because we are replacing them with VRM materials

    getLogger().info("x3");
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

    getLogger().info("x4");

    //Generate Textures

    auto images = doc["images"];
    auto bufferViews = doc["bufferViews"];

    getLogger().info("x5");

    std::vector<UnityEngine::Texture2D*> textures;
    for (size_t i = 0; i < images.size(); i++)
    {
        getLogger().info("x6");
        auto tex = ReadImage(bufferViews[images[i]["bufferView"].get<uint32_t>()], jsonLength, binFile, i);
        UnityEngine::Texture2D* texture = FileToSprite(("sdcard/ModData/vrm_" + std::to_string(i) + ".png"));

        auto bytes = UnityEngine::ImageConversion::EncodeToPNG(texture);

        writefile(("sdcard/ModData/" + std::to_string(i) + ".png").c_str(), std::string((char*) bytes.begin(), bytes.Length()));

        textures.push_back(texture);
    }
    getLogger().info("x7");

    //Generate VRM Materials

    std::vector<UnityEngine::Material*> materials;

    for (size_t i = 0; i < vrm.materialProperties.size(); i++)
    {
        getLogger().info("x8");
        auto material = vrm.materialProperties[i];
        getLogger().info("material %s: %s", material.name.c_str(), material.shader.c_str());
        
        auto mat = UnityEngine::Material::New_ctor(mtoon);

        for (const auto & [ key, value ] : material.floatProperties )
        {
            mat->SetFloat(key.c_str(), value);
        }

        for (const auto & [ key, value ] : material.keywordMap )
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

        for (const auto & [ key, value ] : material.textureProperties )
        {
            mat->SetTexture(key.c_str(), textures[value]);
        }

        for (const auto & [ key, value ] : material.vectorProperties )
        {
            mat->SetColor(key.c_str(), UnityEngine::Color(value[0], value[1], value[2], value[3]));
        }

        for (const auto & [ key, value ] : material.tagMap )
        {
            mat->SetOverrideTag(key, value);
        }

        mat->set_renderQueue(material.renderQueue);

        materials.push_back(mat);
    }
    getLogger().info("x9");

    for (size_t i = 0; i < modelContext->nodes.size(); i++)
    {
        auto node = modelContext->nodes[i];
        if(node->mesh.has_value() && node->processed)
        {
            getLogger().info("x10");
            auto mesh = modelContext->nodes[i]->mesh.value();
            auto material = materials[mesh.materialIdx];
            node->gameObject->GetComponent<UnityEngine::SkinnedMeshRenderer*>()->set_sharedMaterial(material);
        }
    }
    getLogger().info("x11");

    auto avatar = VRM::Mappings::AvatarMappings::CreateAvatar(vrm, modelContext->nodes, modelContext->rootNode->gameObject);

    auto anim = modelContext->rootNode->gameObject->AddComponent<UnityEngine::Animator*>();
    anim->set_avatar(avatar);

    auto vrik = modelContext->rootNode->gameObject->AddComponent<RootMotion::FinalIK::VRIK*>();

    vrik->AutoDetectReferences();

    vrik->solver->spine->headTarget = UnityEngine::GameObject::Find("MenuMainCamera")->get_transform();
    vrik->solver->leftArm->target = UnityEngine::GameObject::Find("ControllerLeft")->get_transform();
    vrik->solver->rightArm->target = UnityEngine::GameObject::Find("ControllerRight")->get_transform();
    vrik->Initiate();
    
    return modelContext;
}