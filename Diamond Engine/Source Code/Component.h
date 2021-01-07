#pragma once

class GameObject;

//ERROR: Remove this include from here
#include"parson/parson.h"

class Component
{
public:

	enum class Type
	{
		None,
		Transform,
		MeshRenderer,
		Material,
		Camera,
		Count,
		Animator,
	};

	Component(GameObject* _gm);
	virtual ~Component();

	virtual void Enable();
	virtual void Disable();

	virtual void Update(float dt);

	virtual bool OnEditor();

	//Maybe return JSON_Value*?
	virtual void SaveData(JSON_Object* nObj);
	virtual void LoadData(JSON_Object* nObj);

	inline bool IsActive() {
		return active;
	}

	inline GameObject* GetGO() {
		return gameObject;
	}

	Type type;

protected:
	//const char* name;
	bool active;
	GameObject* gameObject;
	const char* name;

};