#pragma once
#include <vector>
#include <map>
#include "RE_Animation.h"
#include"MathGeoLib/include/Math/Quat.h"
#include"MathGeoLib/include/Math/float3.h"

class ResourceAnimation;
class aiNode;
class aiAnimation;
class GameObject;

typedef unsigned int uint;

namespace AnimationLoader
{
	void logCallback(const char* message, char* user);

	ResourceAnimation* LoadAnimation(aiAnimation* importedAnimation, uint oldUID = 0);
	uint GetChannelsSize(const Channel& channel);
	void SaveChannels(const Channel& channel, char** cursor);
	void SaveChannelKeys(const std::map<double, float3>& map, char** cursor);
	void SaveChannelKeys(const std::map<double, Quat>& map, char** cursor);

}
