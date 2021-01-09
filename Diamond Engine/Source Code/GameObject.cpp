#include "GameObject.h"
#include "Component.h"

#include "CO_Transform.h"
#include "CO_MeshRenderer.h"
#include "CO_Material.h"
#include "CO_Camera.h"
#include "CO_Animator.h"
#include "RE_Mesh.h"

#include"MaykMath.h"
#include"parson/parson.h"
#include"DEJsonSupport.h"
#include <algorithm>

#include"Application.h"

GameObject::GameObject(const char* _name, GameObject* parent, int _uid) : parent(parent), name(_name), showChildren(false),
active(true), isStatic(false), toDelete(false), UID(_uid), transform(nullptr), dumpComponent(nullptr)
{

	if(parent != nullptr)
		parent->children.push_back(this);

	transform = dynamic_cast<C_Transform*>(AddComponent(Component::Type::Transform));

	//TODO: Should make sure there are not duplicated ID's
	if (UID == -1) 
	{
		UID = EngineExternal->GetRandomInt();
	}
		//UID = MaykMath::Random(0, INT_MAX);
}

GameObject::~GameObject()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		delete components[i];
		components[i] = nullptr;
	}
	components.clear();

	for (size_t i = 0; i < children.size(); i++)
	{
		delete children[i];
		children[i] = nullptr;
	}
	children.clear();
}

void GameObject::Update(float dt)
{
	if (dumpComponent != nullptr) 
	{
		components.erase(std::find(components.begin(), components.end(), dumpComponent));
		delete dumpComponent;
		dumpComponent = nullptr;
	}

	for (size_t i = 0; i < components.size(); i++)
	{
		if(components[i]->IsActive())
			components[i]->Update(dt);
	}
}

Component* GameObject::AddComponent(Component::Type _type)
{

	assert(_type != Component::Type::None, "Can't create a NONE component");
	Component* ret = nullptr;

	//TODO: Make a way to add only 1 instance components like transform and camera
	switch (_type)
	{
	case Component::Type::Transform:
		if(transform == nullptr)
			ret = new C_Transform(this);
		break;
	case Component::Type::MeshRenderer:
		ret = new C_MeshRenderer(this);
		break;
	case Component::Type::Material:
		ret = new C_Material(this);
		break;
	case Component::Type::Camera:
		ret = new C_Camera(this);
		break;
	case Component::Type::Animator:
		ret = new C_Animator(this);
		break;
	}

	if (ret != nullptr)
	{		
		ret->type = _type;
		components.push_back(ret);
	}

	return ret;
}

Component* GameObject::GetComponent(Component::Type _type)
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == _type)
			return components[i];
	}

	return nullptr;
}

bool GameObject::isActive() const
{
	return active;
}

//void GameObject::ChangeActiveState()
//{
//	(active == true) ? Disable() : Enable();
//}

void GameObject::Enable()
{
	active = true;

	if (parent != nullptr)
		parent->Enable();
}

void GameObject::Disable()
{
	active = false;
	//for (size_t i = 0; i < children.size(); i++)
	//{
	//	children[i]->Disable();
	//}
}

bool GameObject::IsRoot()
{
	return (parent == nullptr) ? true : false;
}

void GameObject::Destroy()
{
	toDelete = true;
}

void GameObject::SaveToJson(JSON_Array* _goArray)
{
	JSON_Value* goValue = json_value_init_object();
	JSON_Object* goData = json_value_get_object(goValue);

	//Save all gameObject data
	json_object_set_string(goData, "name", name.c_str());

	DEJson::WriteVector3(goData, "Position", &transform->position[0]);
	DEJson::WriteQuat(goData, "Rotation", &transform->rotation.x);
	DEJson::WriteVector3(goData, "Scale", &transform->localScale[0]);

	DEJson::WriteInt(goData, "UID", UID);

	if (parent)
		DEJson::WriteInt(goData, "ParentUID", parent->UID);

	json_array_append_value(_goArray, goValue);

	//TODO: Move inside component base
	{
		//Save components
		JSON_Value* goArray = json_value_init_array();
		JSON_Array* jsArray = json_value_get_array(goArray);
		for (size_t i = 0; i < components.size(); i++)
		{
			JSON_Value* nVal = json_value_init_object();
			JSON_Object* nObj = json_value_get_object(nVal);

			components[i]->SaveData(nObj);
			json_array_append_value(jsArray, nVal);
		}
		json_object_set_value(goData, "Components", goArray);
	}

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->SaveToJson(_goArray);
	}
}

void GameObject::LoadFromJson(JSON_Object* _obj)
{
	transform->SetTransformMatrix(DEJson::ReadVector3(_obj, "Position"), DEJson::ReadQuat(_obj, "Rotation"), DEJson::ReadVector3(_obj, "Scale"));
	LoadComponents(json_object_get_array(_obj, "Components"));
}

void GameObject::LoadComponents(JSON_Array* componentArray)
{

	DEConfig jsComponent(nullptr);
	for (size_t i = 1; i < json_array_get_count(componentArray); i++)
	{
		jsComponent.nObj = json_array_get_object(componentArray, i);

		Component* comp = AddComponent(static_cast<Component::Type>(jsComponent.ReadInt("Type")));
		comp->LoadData(jsComponent.nObj);

	}

}

void GameObject::RemoveComponent(Component* ptr)
{
	dumpComponent = ptr;
}

//TODO: WTF IS GOING ON WITH THE ARNAU BUG FFS
//Deparenting objects with deformations grows transforms
void GameObject::ChangeParent(GameObject* newParent)
{
	//GameObject* ret = nullptr;
	//ret = IsChild(newParent, ret);
	if (IsChild(newParent))
		return;

	parent->RemoveChild(this);	
	parent = newParent;
	parent->children.push_back(this);

	//TODO: This could be improved, you are setting up the local matrix 2 times
	transform->localTransform = parent->transform->globalTransform.Inverted() * transform->globalTransform;

	Quat _rot;
	float3 scale, pos;
	transform->localTransform.RotatePart().Decompose(_rot, scale);

	transform->SetTransformMatrix(transform->localTransform.TranslatePart(), _rot, scale);
	transform->updateTransform = true;

}

bool GameObject::IsChild(GameObject* _toFind)
{
	if (_toFind == nullptr)
		return false;

	if (_toFind == this)
	{
		return true;
	}
	else
	{
		return IsChild(_toFind->parent);
	}
}

void GameObject::RemoveChild(GameObject* child)
{
	children.erase(std::find(children.begin(), children.end(), child));
}

void GameObject::CollectChilds(std::vector<GameObject*>& vector)
{
	vector.push_back(this);
	for (uint i = 0; i < children.size(); i++)
		children[i]->CollectChilds(vector);
}

void GameObject::GetBoneChildTransforms()
{
	//Get the component and resource mesh;
	GameObject* meshObject = GetFirstChild();
	
	C_MeshRenderer* co_mesh = nullptr;
	co_mesh = dynamic_cast<C_MeshRenderer*>(meshObject->GetComponent(Component::Type::MeshRenderer));
	
	ResourceMesh* animablemesh = co_mesh->GetRenderAnimableMesh();

	//Recursively check bonemap and assign transforms
	RecursiveGetBones(animablemesh, this, meshObject);
}

C_MeshRenderer* GameObject::GetMesh()
{
	//Get the component and resource mesh;
	C_MeshRenderer* co_mesh = nullptr;
	co_mesh = dynamic_cast<C_MeshRenderer*>(this->GetComponent(Component::Type::MeshRenderer));

	return co_mesh;
}

void GameObject::RecursiveGetBones(ResourceMesh* animablemesh, GameObject* root, GameObject* meshObject)
{
			if ((animablemesh->bonesMap.find(root->name.c_str()) != (animablemesh->bonesMap.end())))
			{
				uint bone_ID = animablemesh->bonesMap.find(root->name.c_str())->second;

				C_Transform* transform = dynamic_cast<C_Transform*>(root->GetComponent(Component::Type::Transform));
				animablemesh->bonesTransforms[bone_ID] = transform->GetGlobalMatrix();

				float4x4 delta = CalculateDelta(dynamic_cast<C_Transform*>(meshObject->GetComponent(Component::Type::Transform))->GetGlobalMatrix(), animablemesh->bonesTransforms[bone_ID], animablemesh->bonesOffsets[bone_ID]);
				//Iterate all mesh vertices

				for (uint j = 0; j < animablemesh->bones_count; ++j)
				{
					//Iterate all 4 bones
					for (uint k = 0; k < 4; ++k)
					{
						int boneID = animablemesh->bones[j * 4 + k];
						float boneWeight = animablemesh->boneWeights[j * 4 + k];
				
						if (boneID == -1) continue;
				
						float3 vertex = delta.TransformPos(float3(&animablemesh->vertices[j * 3]));
				
						animablemesh->vertices[j * 3] += vertex.x * boneWeight;
						animablemesh->vertices[j * 3 + 1] += vertex.y * boneWeight;
						animablemesh->vertices[j * 3 + 2] += vertex.z * boneWeight;
						LOG(LogType::L_ERROR, "J: %i", j);
				
						if (animablemesh->normals_count > 0) {
							float3 normal = delta.TransformPos(float3(&animablemesh->normals[j * 3]));
							animablemesh->normals[j * 3] += normal.x * boneWeight;
							animablemesh->normals[j * 3 + 1] += normal.y * boneWeight;
							animablemesh->normals[j * 3 + 2] += normal.z * boneWeight;
						}
					}
				}
			}	

			for (int i = 0; i < root->children.size(); i++)
			{
				RecursiveGetBones(animablemesh, root->children[i], meshObject);
			}
}

float4x4 GameObject::CalculateDelta(float4x4 meshGlobal, float4x4 boneGlobal, float4x4 Offset)
{
	float4x4 meshInverted = meshGlobal.Inverted();
	float4x4 boneLocal = meshInverted * boneGlobal;
	float4x4 Delta = boneLocal * Offset;
	return Delta;
}


GameObject* GameObject::GetFirstChild()
{
	return this->children[0];
}