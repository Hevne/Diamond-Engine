#pragma once
#include "Component.h"
#include <vector>
#include <map>
#include <string>

class GameObject;
class Channel;
class ResourceAnimation;
class Resource;

typedef unsigned int uint;


class C_Animator : public Component
{
public:
	C_Animator(GameObject* gameObject);
	~C_Animator();

	//void DrawLinkedBones() const;

	//void Start();
	//void Update(float dt);

	//void SetAnimation(const char* name, float blendTime = 0.0f);
	//void SetAnimation(uint index, float blendTime = 0.0f);

	//ResourceAnimation* GetAnimation(uint index);

	//void SetResource(Resource* resource);
	//void SetResource(unsigned long long ID);

	////uint64 GetResourceID() const;
	//static inline Type GetType() { return Type::Animator; };

private:


public:
	GameObject* rootBone = nullptr;
	std::map<std::string, GameObject*> boneMapping;

	//Used for blending
	uint previous_animation = 0;
	uint current_animation = 0;
	bool playing = false;

private:
	bool started = false;

	float prevAnimTime = 0.0f;
	float time = 0.0f;
	float blendTime = 0.0f;
	float blendTimeDuration = 0.0f;

	bool channelsLinked = false;
	bool bonesLinked = false;

};
