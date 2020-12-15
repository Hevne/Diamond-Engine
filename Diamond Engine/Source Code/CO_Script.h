#pragma once
#include "Component.h"
#include<vector>
#include<string>

#include"MO_MonoManager.h"
#include <mono/metadata/object-forward.h>

class GameObject;
typedef struct _MonoClassField MonoClassField;

class C_Script : public Component
{
public:

	C_Script(GameObject* _gm);
	virtual ~C_Script();

	void Update() override;

	bool OnEditor() override;

	void DropField(SerializedField& fieldName, const char* dropType);
	void SetField(MonoClassField* field, GameObject* value);

	std::vector<std::string> methods;
	std::vector<SerializedField> fields;

	MonoObject* coreObject;
	MonoMethod* updateMethod;

	GameObject* temporalReference;
};