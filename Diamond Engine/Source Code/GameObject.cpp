#include "GameObject.h"
#include "Component.h"

#include "C_Transform.h"
#include "C_MeshRenderer.h"
#include "C_Material.h"

#include"MaykMath.h"
#include"parson/parson.h"
#include <algorithm>

GameObject::GameObject(const char* _name, GameObject* parent, int _uid) : parent(parent), name(_name), showChildren(false),
active(true), isStatic(false), toDelete(false), UID(_uid), transform(nullptr), dumpComponent(nullptr)
{
	transform = dynamic_cast<C_Transform*>(AddComponent(Component::Type::Transform));

	//TODO: Should make sure there are not duplicated ID's
	if(UID == -1)
		UID = MaykMath::Random(0, INT_MAX);
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

void GameObject::Update()
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
			components[i]->Update();
	}
}

Component* GameObject::AddComponent(Component::Type _type)
{

	assert(_type != Component::Type::None, "Can't create a NONE component");
	Component* ret = nullptr;

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

void GameObject::LoadComponents(JSON_Array* componentArray)
{

	JSON_Object* jsComponent = nullptr;
	for (size_t i = 1; i < json_array_get_count(componentArray); i++)
	{
		jsComponent = json_array_get_object(componentArray, i);

		Component* comp = AddComponent((Component::Type)json_object_get_number(jsComponent, "Type"));
		comp->LoadData(jsComponent);

	}

}

void GameObject::RemoveComponent(Component* ptr)
{
	dumpComponent = ptr;
}
