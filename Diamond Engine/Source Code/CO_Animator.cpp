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

C_Animator::C_Animator(GameObject* gameobject) : Component(gameobject)
{
	gameObject = gameobject;
	name = "Animator Component";
}

C_Animator::~C_Animator()
{
	if (_anim != nullptr)
	{
		EngineExternal->moduleResources->UnloadResource(_anim->GetUID());
		_anim = nullptr;
	}
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
	else
	{
		//LinkChannelBones(gameObject);
	}
}

bool C_Animator::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		if (_anim != nullptr)
		{
			ImGui::Text("Previous Animation: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", previous_animation);
			ImGui::Text("Current Animation: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", current_animation);
			ImGui::Text("Previous Animation Time: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", prevAnimTime);
			ImGui::Text("Current Animation Time: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", time);
			ImGui::Text("blendTime: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", blendTime);

			ImGui::Spacing();
			if (playing)
			{
				ImGui::Text("Playing: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "true");
			}
			else
			{
				ImGui::Text("Playing: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "false");
			}
			if (channeIsLinked)
			{
				ImGui::Text("Channel Linked: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "true");
			}
			else
			{
				ImGui::Text("Channel Linked: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "false");
			}
			if (bonesLinked)
			{
				ImGui::Text("Bones Linked: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "true");
			}
			else
			{
				ImGui::Text("Bones Linked: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "false");
			}

			ImGui::Spacing();

			if (ImGui::Button("Link channels to bones")) {
				//LinkChannelBones(gameObject);
				playing = true;
			}
		

			ImGui::Spacing();

			ImGui::Text("Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", _anim->GetLibraryPath());
		}
		return true;
	}
	return false;
}

void C_Animator::LinkChannelBones(GameObject* gameObject)
{
	for (int i = 0; i < gameObject->children.size(); i++)
	{
		if (_anim->channels.find(gameObject->children[i]->name) != _anim->channels.end()) {
			LOG(LogType::L_WARNING, "Found match between bone-channel");
			rootBone = gameObject->children[i];
			channeIsLinked = true;
			StoreBoneMapping(gameObject);
		}
		LinkChannelBones(gameObject->children[i]);
	}
}

void C_Animator::StoreBoneMapping(GameObject* gameObject)
{
	boneMapping[gameObject->name] = gameObject;
	for (int i = 0; i < gameObject->children.size(); i++)
	{
		StoreBoneMapping(gameObject->children[i]);
	}
}



void C_Animator::SetAnimation(ResourceAnimation* anim)
{
	_anim = anim;
}

void C_Animator::Start()
{
	//TODO: hard-coding root bone for fast code iteration
	rootBone = gameObject->children[1];
	//gameObject->children[0]->GetComponent<C_Mesh>()->rootBone = rootBone;

	if (rootBone == nullptr) return;

	boneMapping.clear();

	std::vector<GameObject*> bones;
	rootBone->CollectChilds(bones);

	for (uint i = 0; i < bones.size(); ++i)
	{
		boneMapping[bones[i]->name] = bones[i];
	}
}

void C_Animator::Update(float dt)
{
	if (playing == true) {
		if (started == false) {
			Start();
		}

		ResourceAnimation* currentAnimation = _anim;

		time += dt;

		if (currentAnimation && time > currentAnimation->duration) {
			if (currentAnimation->loopable == true) {
				time = 0.0f;
			}
			else {
				playing = false;
				return;
			}
		}
	
	}
}


