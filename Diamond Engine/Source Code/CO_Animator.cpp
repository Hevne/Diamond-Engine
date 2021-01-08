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

void C_Animator::Start()
{
	//hard coded first bone
	rootBone = gameObject->children[1]->children[0];
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
		currentTimeAnimation = time * currentAnimation->ticksPerSecond;
		if (currentAnimation && currentTimeAnimation > currentAnimation->duration) {
			if (currentAnimation->loopable == true) {
				time = 0.0f;
			}
			else {
				playing = false;
				return;
			}
		}

		UpdateChannelsTransform(currentAnimation, nullptr, 0.f);
		std::vector<GameObject*> bones;
		rootBone->CollectChilds(bones);
		DrawBones(bones[0]);
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
			ImGui::Text("Previous Animation Time: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%f", prevAnimTime);
			ImGui::Text("Current Animation Time: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", currentTimeAnimation);
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

void C_Animator::UpdateChannelsTransform(const ResourceAnimation* settings, const ResourceAnimation* blend, float blendRatio)
{
	uint currentFrame = currentTimeAnimation;
	//LOG(LogType::L_NORMAL, "%i", currentFrame);
	uint prevBlendFrame = 0;
	if (blend != nullptr)
	{
		prevBlendFrame = blend->ticksPerSecond * prevAnimTime;
	}

	std::map<std::string, GameObject*>::iterator boneIt;
	for (boneIt = boneMapping.begin(); boneIt != boneMapping.end(); ++boneIt)
	{
		C_Transform* transform = dynamic_cast<C_Transform*>(boneIt->second->GetComponent(Component::Type::Transform));

		if (settings->channels.find(boneIt->first.c_str()) == settings->channels.end()) continue;

		const Channel& channel = settings->channels.find(boneIt->first.c_str())->second;
	
		float3 position = GetChannelPosition(channel, currentFrame, transform->position);
		Quat rotation = GetChannelRotation(channel, currentFrame, transform->rotation);
		float3 scale = GetChannelScale(channel, currentFrame, transform->localScale);


		//BLEND
		if (blend != nullptr)
		{
			const Channel& blendChannel = blend->channels.find(boneIt->first.c_str())->second;

			position = float3::Lerp(GetChannelPosition(blendChannel, prevBlendFrame, transform->position), position, blendRatio);
			rotation = Quat::Slerp(GetChannelRotation(blendChannel, prevBlendFrame, transform->rotation), rotation, blendRatio);
			scale = float3::Lerp(GetChannelScale(blendChannel, prevBlendFrame, transform->localScale), scale, blendRatio);
		}

		
		transform->position = position;
		transform->eulerRotation = rotation.ToEulerXYZ() * RADTODEG;
		transform->localScale = scale;
		transform->updateTransform = true;
	}
}

Quat C_Animator::GetChannelRotation(const Channel& channel, float currentKey, Quat default) const
{
	Quat rotation = default;

	if (channel.rotationKeys.size() > 0)
	{
		std::map<double, Quat>::const_iterator previous = channel.GetPrevRotKey(currentKey);
		std::map<double, Quat>::const_iterator next = channel.GetNextRotKey(currentKey);

		if (next == channel.rotationKeys.end())
			next = previous;

		Quat quatLog = previous->second;
		//LOG(LogType::L_NORMAL, "Frame: %2.f Quat(%f,%f,%f,%f)", previous->first,quatLog.x,quatLog.y,quatLog.z,quatLog.w);
		//If both keys are the same, no need to blend
		if (previous == next)
			rotation = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			rotation = previous->second.Slerp(next->second, ratio);
		}
	}
	return rotation;
}
float3 C_Animator::GetChannelPosition(const Channel& channel, float currentKey, float3 default) const
{
	float3 position = default;

	if (channel.positionKeys.size() > 0)
	{
		std::map<double, float3>::const_iterator previous = channel.GetPrevPosKey(currentKey);
		std::map<double, float3>::const_iterator next = channel.GetNextPosKey(currentKey);

		if (next == channel.positionKeys.end())
			next = previous;

		//If both keys are the same, no need to blend
		if (previous == next)
			position = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			position = previous->second.Lerp(next->second, ratio);
		}
	}

	return position;
}


float3 C_Animator::GetChannelScale(const Channel & channel, float currentKey, float3 default) const
{
	float3 scale = default;

	if (channel.scaleKeys.size() > 0)
	{
		std::map<double, float3>::const_iterator previous = channel.GetPrevScaleKey(currentKey);
		std::map<double, float3>::const_iterator next = channel.GetPrevScaleKey(currentKey);

		if (next == channel.scaleKeys.end())
			next = previous;

		//If both keys are the same, no need to blend
		if (previous == next)
			scale = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			scale = previous->second.Lerp(next->second, ratio);
		}
	}
	return scale;
}

void C_Animator::DrawBones(GameObject* gameObject)
{
	glColor3f(1.f, 0.f, 0.f);
	glLineWidth(4.f);
	glBegin(GL_LINES);

	//Draw lines
	std::map<std::string, GameObject*>::iterator bones;
	float3 position;
	Quat rotation;
	float3 scale;

	gameObject->transform->globalTransform.Decompose(position, rotation, scale);
	glVertex3f(position.x, position.y, position.z);
	//LOG(LogType::L_NORMAL, "Name: %s  %f,%f,%f",bones->first.c_str(), position.x, position.y, position.z);
	if (gameObject->children.size() > 0) {
		for (uint i = 0; i < gameObject->children.size(); i++)
		{
			DrawBones(gameObject->children[i]);
		}
	}
	glEnd();
	glLineWidth(1.f);
	glColor3f(1.f, 1.f, 1.f);
}

