#include "customTypes/DeadSimpleShadows/DrawObject.hpp"

#include "System/Collections/Generic/List_1.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp"
#include "customTypes/DeadSimpleShadows/DrawSystem.hpp"
#include "main.hpp"

namespace DeadSimpleShadows
{
    CP_SDK_IL2CPP_INHERIT_INIT(DrawObject);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(DrawObject)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(DrawObject)
    {

    }

    void DrawObject::CheckChildren()
    {
        if (responsibleMeshRenderers == nullptr || responsibleMeshRenderers->Count == 0)
        {
            responsibleMeshRenderers = System::Collections::Generic::List_1<UnityEngine::Renderer*>::New_ctor();
            GetComponentsInChildren(responsibleMeshRenderers); // Grabs Mesh Renderers from children.
        }
    }

    void DrawObject::OnDisable()
    {
        if (responsibleMeshRenderers != nullptr)
        {
            responsibleMeshRenderers->Clear();
        }
    }

    void DrawObject::OnEnable()
    {
        CheckChildren();
        DrawRegistry::Add(this);
    }

    void DrawObject::Start()
    {
        CheckChildren();
        DrawRegistry::Remove(this);
    }
}