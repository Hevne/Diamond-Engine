#pragma once


#include  <vector>
#include <string>
#include <map>

#include"MathGeoLib/include/MathGeoLib.h"
#include "DEResource.h"

typedef unsigned int GLuint;
typedef unsigned int uint;

struct Channel
{
	std::string boneName;

	//Time & Value
	std::map<double, float3> positionKeys;
	std::map<double, Quat> rotationKeys;
	std::map<double, float3> scaleKeys;
};

class ResourceAnimation : public Resource
{

public:
	ResourceAnimation(unsigned int _uid);
	~ResourceAnimation();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;



public:

	std::string animationName;
	float duration;
	float ticksPerSecond;
	bool loopable = true;
	uint numChannels;

	std::map<std::string, Channel> channels;
};