#include "main.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Rendering/IndexFormat.hpp"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"

using namespace tinygltf;
using namespace UnityEngine;

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

//Based off https://github.com/KhronosGroup/UnityGLTF/blob/master/UnityGLTF/Assets/UnityGLTF/Runtime/Scripts/GLTFSceneImporter.cs#L41
struct UnityMeshData
{
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector4> tangents;
    std::vector<Vector2> uv1;
    std::vector<Vector2> uv2;
    std::vector<Vector2> uv3;
    std::vector<Vector2> uv4;
    std::vector<Color> colors;
    std::vector<BoneWeight> boneWeights;

    std::vector<std::vector<Vector3>> morphTargetVertices;
    std::vector<std::vector<Vector3>> morphTargetNormals;
    std::vector<std::vector<Vector3>> morphTargetTangents;

    std::vector<MeshTopology> topology;
    std::vector<std::vector<int>> indices;
};

int getVertexCount(tinygltf::Primitive primitive, tinygltf::Model model)
{
    const tinygltf::Accessor& accessor = model.accessors[primitive.attributes["POSITION"]];
    return accessor.count;
}

bool hasAttribKey(std::string key, tinygltf::Primitive primitive)
{
    return primitive.attributes.contains(key);
}

template <typename T>
void allocateInto2DArray(int x, int y, std::vector<std::vector<T>>& vector)
{
    vector = std::vector<std::vector<T>>(x);
    for (int i = 0; i < x; i++)
    {
        vector[i] = std::vector<T>(y);
    }
}

MeshTopology getTopology(int mode)
{
    switch (mode)
    {
        case TINYGLTF_MODE_POINTS: return MeshTopology::Points;
        case TINYGLTF_MODE_LINE: return MeshTopology::Lines;
        case TINYGLTF_MODE_LINE_STRIP: return MeshTopology::LineStrip;
        case TINYGLTF_MODE_TRIANGLES: return MeshTopology::Triangles;
    }
    getLogger().error("mode does not exist: %d", mode);
    return MeshTopology::Points;
}

std::vector<int> generateIndices(int vertCount)
{
	auto indices = std::vector<int>(vertCount);
	for (auto i = 0; i < vertCount; i++)
	{
		indices[i] = i;
	}

	return indices;
}

void flipTriangleFaces(std::vector<int>& indices)
{
	for (int i = 0; i < indices.size(); i += 3)
	{
		int temp = indices[i];
		indices[i] = indices[i + 2];
		indices[i + 2] = temp;
	}
}

bool approx(float one, float two)
{
    return (std::abs(one - two) < 0.001f);
}

Vector4 divideVec4(Vector4 vector, float value)
{
    return Vector4(vector.x / value, vector.y / value, vector.z / value, vector.w / value);
}

void createBoneWeightArray(std::vector<Vector4> joints, std::vector<Vector4> weights, std::vector<BoneWeight>& destArr, int offset = 0)
{
	// normalize weights (built-in normalize function only normalizes three components)
	for (int i = 0; i < weights.size(); i++)
	{
		auto weightSum = (weights[i].x + weights[i].y + weights[i].z + weights[i].w);

		if (!approx(weightSum, 0.0f))
		{
			weights[i] = divideVec4(weights[i], weightSum);
		}
	}

	for (int i = 0; i < joints.size(); i++)
	{
		destArr[offset + i].set_boneIndex0((int)joints[i].x);
		destArr[offset + i].set_boneIndex1((int)joints[i].y);
		destArr[offset + i].set_boneIndex2((int)joints[i].z);
		destArr[offset + i].set_boneIndex3((int)joints[i].w);

		destArr[offset + i].set_weight0(weights[i].x);
		destArr[offset + i].set_weight1(weights[i].y);
		destArr[offset + i].set_weight2(weights[i].z);
		destArr[offset + i].set_weight3(weights[i].w);
	}
}

//https://github.com/syoyo/tinygltf/wiki/Accessing-vertex-data
std::vector<Vector3> getVec3Data(tinygltf::Model model, tinygltf::Primitive primitive, std::string key)
{
    tinygltf::Accessor& accessor = model.accessors[primitive.attributes[key]];
    tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
    tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
    float* values = reinterpret_cast<float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

    std::vector<Vector3> positions = std::vector<Vector3>(accessor.count);
    for (size_t i = 0; i < accessor.count; ++i) {
         float x = values[i * 3 + 0];
         float y = values[i * 3 + 1];
         float z = values[i * 3 + 2];
         positions[i] = Vector3(x, y, z);
    }
    return positions;
}

std::vector<Vector3> getVec3DataTarget(tinygltf::Model model, int target)
{
    tinygltf::Accessor& accessor = model.accessors[target];
    tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
    tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
    float* values = reinterpret_cast<float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

    std::vector<Vector3> positions = std::vector<Vector3>(accessor.count);
    for (size_t i = 0; i < accessor.count; ++i) {
         float x = values[i * 3 + 0];
         float y = values[i * 3 + 1];
         float z = values[i * 3 + 2];
         positions[i] = Vector3(x, y, z);
    }
    return positions;
}

std::vector<Vector2> getVec2Data(tinygltf::Model model, tinygltf::Primitive primitive, std::string key)
{
    tinygltf::Accessor& accessor = model.accessors[primitive.attributes[key]];
    tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
    tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
    float* values = reinterpret_cast<float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

    std::vector<Vector2> positions = std::vector<Vector2>(accessor.count);
    for (size_t i = 0; i < accessor.count; ++i) {
         float x = values[i * 2 + 0];
         float y = values[i * 2 + 1];
         positions[i] = Vector2(x, y);
    }
    return positions;
}

std::vector<Vector4> getVec4Data(tinygltf::Model model, tinygltf::Primitive primitive, std::string key)
{
    tinygltf::Accessor& accessor = model.accessors[primitive.attributes[key]];
    tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
    tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
    float* values = reinterpret_cast<float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

    std::vector<Vector4> positions = std::vector<Vector4>(accessor.count);
    for (size_t i = 0; i < accessor.count; ++i) {
         float x = values[i * 4 + 0];
         float y = values[i * 4 + 1];
         float z = values[i * 4 + 2];
         float w = values[i * 4 + 3];
         positions[i] = Vector4(x, y, z, w);
    }
    return positions;
}

std::vector<Color> getColorData(tinygltf::Model model, tinygltf::Primitive primitive, std::string key)
{
    tinygltf::Accessor& accessor = model.accessors[primitive.attributes[key]];
    tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
    tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
    float* values = reinterpret_cast<float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

    bool hasAlpha = bufferView.byteStride > 3;

    std::vector<Color> positions = std::vector<Color>(accessor.count);
    for (size_t i = 0; i < accessor.count; ++i) {
         float x = values[i * bufferView.byteStride + 0];
         float y = values[i * bufferView.byteStride + 1];
         float z = values[i * bufferView.byteStride + 2];
         float a = hasAlpha ? values[i * bufferView.byteStride + 3] : 1.0f;
         positions[i] = Color(x, y, z, a);
    }
    return positions;
}

std::vector<int> getIntData(tinygltf::Model model, tinygltf::Primitive primitive, std::string key)
{
    tinygltf::Accessor& accessor = model.accessors[primitive.attributes[key]];
    tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
    tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
    int* values = reinterpret_cast<int*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

    std::vector<int> data = std::vector<int>(accessor.count);
    for (size_t i = 0; i < accessor.count; ++i) {
         int x = values[i];
         data[i] = x;
    }
    return data;
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

void convertAttributeAccessorsToUnityTypes(tinygltf::Primitive primData, tinygltf::Model model, UnityMeshData& unityData, int vertOffset, int indexOffset)
{
    auto meshAttributes = primData.attributes;
    int vertexCount = getVertexCount(primData, model);

    const tinygltf::Accessor& indicesAccessor = model.accessors[meshAttributes["INDICES"]];
    
    auto indices = meshAttributes.contains("INDICES")
         ? getIntData(model, primData, "INDICES")
         : generateIndices(vertexCount);

    if (unityData.topology[indexOffset] == MeshTopology::Triangles)
        flipTriangleFaces(indices);
    
    unityData.indices[indexOffset] = indices;

    if (meshAttributes.contains("WEIGHTS_0") && meshAttributes.contains("JOINTS_0"))
    {
        createBoneWeightArray(
            getVec4Data(model, primData, "JOINTS_0"),
            getVec4Data(model, primData, "WEIGHTS_0"),
            unityData.boneWeights,
            vertOffset);
    }

    if (meshAttributes.contains("POSITION"))
    {
        unityData.vertices = getVec3Data(model, primData, "POSITION");
    }
    if (meshAttributes.contains("NORMAL"))
    {
        unityData.normals = getVec3Data(model, primData, "NORMAL");
    }
    if (meshAttributes.contains("TANGENT"))
    {
        unityData.tangents = getVec4Data(model, primData, "TANGENT");
    }
    if (meshAttributes.contains("TEXCOORD_0"))
    {
        unityData.uv1 = getVec2Data(model, primData, "TEXCOORD_0");
    }
    if (meshAttributes.contains("TEXCOORD_1"))
    {
        unityData.uv2 = getVec2Data(model, primData, "TEXCOORD_1");
    }
    if (meshAttributes.contains("TEXCOORD_2"))
    {
        unityData.uv3 = getVec2Data(model, primData, "TEXCOORD_2");
    }
    if (meshAttributes.contains("TEXCOORD_3"))
    {
        unityData.uv4 = getVec2Data(model, primData, "TEXCOORD_3");
    }
    if (meshAttributes.contains("COLOR_0"))
    {
        unityData.colors = getColorData(model, primData, "COLOR_0");
    }

    auto targets = primData.targets;
    if (targets.size() > 0)
    {
        for (int i = 0; i < targets.size(); ++i)
        {
            if (targets[i].contains("POSITION"))
            {    
                unityData.morphTargetVertices[i] = getVec3DataTarget(model, targets[i]["POSITION"]);
            }
            if (targets[i].contains("NORMAL"))
            {
                unityData.morphTargetNormals[i] = getVec3DataTarget(model, targets[i]["NORMAL"]);
            }
            if (targets[i].contains("TANGENT"))
            {
                unityData.morphTargetTangents[i] = getVec3DataTarget(model, targets[i]["TANGENT"]);
            }
        }
    }
}

MAKE_HOOK_MATCH(MainMenuUIHook, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuUIHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    getLogger().info("Starting Load!");

    Model model;
    TinyGLTF loader;
    std::string err;
    std::string warn;

    getLogger().info("file exists: %s", exists("sdcard/ModData/ava.vrm") ? "true" : "false");
    
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, "sdcard/ModData/ava.vrm");

    if (!warn.empty()) {
        getLogger().info("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        getLogger().info("Err: %s\n", err.c_str());
    }

    if (!ret) {
        getLogger().info("Failed to parse glTF\n");
    }

    getLogger().info("%lu", model.meshes.size());
    getLogger().info("%lu", model.textures.size());
    getLogger().info("%lu", model.extensions.size());
    getLogger().info("%lu", model.animations.size());
    getLogger().info("%lu", model.materials.size());
    getLogger().info("%lu", model.images.size());
    getLogger().info("%lu", model.skins.size());
    getLogger().info("%lu", model.scenes.size());

    int idx = 0;
    for (auto texture : model.textures)
    {
        idx++;
        getLogger().info("Image %d", idx);
        tinygltf::Image &image = model.images[texture.source];
        getLogger().info(" - dimensions: %dx%d", image.width, image.height);
        stbi_write_png(("sdcard/ModData/Image" + std::to_string(idx) + ".png").c_str(), image.width, image.height, image.component, &image.image[0], 0);
    }
    
    auto mesh = model.meshes[0];
    getLogger().info("mesh 0 name: %s", mesh.name.c_str());
    getLogger().info("mesh 0 primatives: %lu", mesh.primitives.size());

    int vertexCount = 0;
    for (auto primitive : mesh.primitives)
    {
        vertexCount += getVertexCount(primitive, model);
    }
    auto vertOffset = 0;
    auto firstPrim = mesh.primitives[0];

    auto meshData = UnityMeshData();

    getLogger().info("Starting Intermediate Mesh Construction");

    meshData.vertices = std::vector<Vector3>(vertexCount);
    meshData.normals = hasAttribKey("NORMAL", firstPrim) ? std::vector<Vector3>(vertexCount) : std::vector<Vector3>();
    meshData.tangents = hasAttribKey("TANGENT", firstPrim) ? std::vector<Vector4>(vertexCount) : std::vector<Vector4>();

    meshData.uv1 = hasAttribKey("TEXCOORD_0", firstPrim) ? std::vector<Vector2>(vertexCount) : std::vector<Vector2>();
    meshData.uv2 = hasAttribKey("TEXCOORD_1", firstPrim) ? std::vector<Vector2>(vertexCount) : std::vector<Vector2>();
    meshData.uv3 = hasAttribKey("TEXCOORD_2", firstPrim) ? std::vector<Vector2>(vertexCount) : std::vector<Vector2>();
    meshData.uv4 = hasAttribKey("TEXCOORD_3", firstPrim) ? std::vector<Vector2>(vertexCount) : std::vector<Vector2>();

    meshData.colors = hasAttribKey("COLOR_0", firstPrim) ? std::vector<Color>(vertexCount) : std::vector<Color>();
    meshData.boneWeights = hasAttribKey("WEIGHTS_0", firstPrim) ? std::vector<BoneWeight>(vertexCount) : std::vector<BoneWeight>();
    
    getLogger().info("Intermediate Mesh initialized, initializing morph values");

    if(firstPrim.targets.size() > 0 && firstPrim.targets[0].contains("POSITION"))
    {
        allocateInto2DArray<Vector3>((int)firstPrim.targets.size(), vertexCount, meshData.morphTargetVertices);
    }
    else
    {
        meshData.morphTargetVertices = std::vector<std::vector<Vector3>>();
    }

    if(firstPrim.targets.size() > 0 && firstPrim.targets[0].contains("NORMAL"))
    {
        allocateInto2DArray<Vector3>((int)firstPrim.targets.size(), vertexCount, meshData.morphTargetNormals);
    }
    else
    {
        meshData.morphTargetNormals = std::vector<std::vector<Vector3>>();
    }

    if(firstPrim.targets.size() > 0 && firstPrim.targets[0].contains("TANGENT"))
    {
        allocateInto2DArray<Vector3>((int)firstPrim.targets.size(), vertexCount, meshData.morphTargetTangents);
    }
    else
    {
        meshData.morphTargetTangents = std::vector<std::vector<Vector3>>();
    }

    getLogger().info("morph values initialized");

    meshData.topology = std::vector<MeshTopology>(mesh.primitives.size());
    meshData.indices = std::vector<std::vector<int>>(mesh.primitives.size());

    getLogger().info("initialized topology and indices, loading primitives");

    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
        getLogger().info("primitive %lu", i);
        auto primitive = mesh.primitives[i];

        meshData.topology[i] = getTopology(primitive.mode);
        getLogger().info("- primitive topology %d", (int)meshData.topology[i]);

        convertAttributeAccessorsToUnityTypes(primitive, model, meshData, vertOffset, i);

        getLogger().info("- converted attribute accessors :D");

        auto vertCount = getVertexCount(primitive, model);
		vertOffset += (int)vertCount;
        getLogger().info("- primitive finished :DDDD");
    }
    getLogger().info("Loaded all primitives, time to construct unity mesh");

    //ConstructUnityMesh

    UnityEngine::Mesh* unityMesh = UnityEngine::Mesh::New_ctor();
    unityMesh->set_name("Mesh 0");
    unityMesh->set_indexFormat(meshData.vertices.size() > 65535 ? UnityEngine::Rendering::IndexFormat::UInt32 : UnityEngine::Rendering::IndexFormat::UInt16);
    
    getLogger().info("Mesh object created, setting values");

    unityMesh->set_vertices(vector2ArrayW(meshData.vertices));
    unityMesh->set_normals(vector2ArrayW(meshData.normals));
    unityMesh->set_tangents(vector2ArrayW(meshData.tangents));
    unityMesh->set_uv(vector2ArrayW(meshData.uv1));
    unityMesh->set_uv2(vector2ArrayW(meshData.uv2));
    unityMesh->set_uv3(vector2ArrayW(meshData.uv3));
    unityMesh->set_uv4(vector2ArrayW(meshData.uv4));
    unityMesh->set_colors(vector2ArrayW(meshData.colors));
    unityMesh->set_boneWeights(vector2ArrayW(meshData.boneWeights));

    getLogger().info("values setting, loading in indices");

    unityMesh->set_subMeshCount(meshData.indices.size());
    uint baseVertex = 0;
	for (int i = 0; i < meshData.indices.size(); i++)
	{
		unityMesh->SetIndices(ArrayW<int>(vector2ArrayW(meshData.indices[i])), meshData.topology[i], i, false, (int)baseVertex);
		baseVertex += getVertexCount(mesh.primitives[i], model);
	}
    unityMesh->RecalculateBounds();

    getLogger().info("indices loaded, loading morph targets");

    if (meshData.morphTargetVertices.size() > 0)
	{
		for (int i = 0; i < firstPrim.targets.size(); i++)
		{
		    auto targetName = "Morphtarget " + std::to_string(i);
			unityMesh->AddBlendShapeFrame(targetName, 100,
				vector2ArrayW(meshData.morphTargetVertices[i]),
				meshData.morphTargetNormals.size() > 0 ? vector2ArrayW(meshData.morphTargetNormals[i]) : nullptr,
				meshData.morphTargetTangents.size() > 0 ? vector2ArrayW(meshData.morphTargetTangents[i]) : nullptr
			);
		}
	}

    getLogger().info("morph targets loaded, recalculating any missing normals");

	if (meshData.normals.size() < 1 && meshData.topology[0] == MeshTopology::Triangles)
	{
		unityMesh->RecalculateNormals();
	}

    getLogger().info("Unity Mesh Constructed :DDDDDDDDDD, creating renderer");

    auto Root = GameObject::New_ctor("GLTF");
    GameObject::DontDestroyOnLoad(Root);
    auto meshObj = GameObject::New_ctor("mesh");
    meshObj->get_transform()->set_parent(Root->get_transform());
    
    getLogger().info("obj created, adding components");

    auto filter = meshObj->AddComponent<UnityEngine::MeshFilter*>();
    auto renderer = meshObj->AddComponent<UnityEngine::MeshFilter*>();

    filter->set_mesh(unityMesh);

    getLogger().info("mesh components setup");
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