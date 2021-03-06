#pragma once

#include<vector>
#include<string>

#include "Component.h"
#include "CO_MeshRenderer.h"
#include "DEResource.h"
#include "RE_Mesh.h"

#include"parson/parson.h"

class C_Transform;

class GameObject
{
public:

	GameObject(const char*, GameObject* parent, int _uid = -1);
	virtual ~GameObject();

	void Update(float dt);

	Component* AddComponent(Component::Type _type);
	Component* GetComponent(Component::Type _type);

	bool isActive() const;

	void Enable();
	void Disable();
	bool IsRoot();

	void Destroy();

	void SaveToJson(JSON_Array*);
	void LoadFromJson(JSON_Object*);

	void LoadComponents(JSON_Array* componentArray);
	void RemoveComponent(Component* ptr);

	void ChangeParent(GameObject*);
	bool IsChild(GameObject*);

	void RemoveChild(GameObject*);
	void CollectChilds(std::vector<GameObject*>& vector);

	void GetBoneChildTransforms();
	C_MeshRenderer* GetMesh();
	void RecursiveGetBones(ResourceMesh* animablemesh, GameObject* root, GameObject* meshObject);
	float4x4 CalculateDelta(float4x4 meshGlobal, float4x4 boneGlobal, float4x4 Offset);
	GameObject* GetFirstChild();

	GameObject* parent;
	C_Transform* transform;

	std::vector<GameObject*> children;

	std::vector<Component*> components;

public:
	std::string name;
	bool active, isStatic;

	//TODO: Temporal tree display bool
	bool showChildren;
	bool toDelete;

	int UID;

private:
	Component* dumpComponent;

};