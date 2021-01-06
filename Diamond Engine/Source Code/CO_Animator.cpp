#include "CO_Animator.h"
#include "RE_Animation.h"
#include "OpenGL.h"

#include "Application.h"
#include "MO_Renderer3D.h"
#include "IM_FileSystem.h"
#include"MO_ResourceManager.h"

#include "GameObject.h"
#include "CO_Material.h"
#include "CO_Transform.h"
#include"CO_Camera.h"
#include "CO_MeshRenderer.h"

#include "ImGui/imgui.h"
#include"DEJsonSupport.h"

#include"MathGeoLib/include/Geometry/Frustum.h"
#include"MathGeoLib/include/Geometry/Plane.h"

C_Animator::C_Animator(GameObject* gameObject) : Component(gameObject)
{
}

C_Animator::~C_Animator()
{
}

void C_Animator::SetResource(ResourceAnimation* re_anim)
{
	_anim = re_anim;
}

void C_Animator::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);
	DEJson::WriteString(nObj, "Path", _anim->GetLibraryPath());
	DEJson::WriteInt(nObj, "UID", _anim->GetUID());
}

void C_Animator::LoadData(JSON_Object* nObj)
{
	Component::LoadData(nObj);

	DEConfig jsObj(nObj);

	SetAnimation(dynamic_cast<ResourceAnimation*>(EngineExternal->moduleResources->RequestResource(jsObj.ReadInt("UID"), jsObj.ReadString("Path"))));

	if (_anim == nullptr)
		return;
}

void C_Animator::SetAnimation(ResourceAnimation* anim)
{
	_anim = anim;
}
