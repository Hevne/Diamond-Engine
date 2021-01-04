#include "IM_AnimationImporter.h"
#include "Application.h"
#include "MO_Editor.h"

#include "GameObject.h"
#include "CO_Transform.h"

#include "RE_Animation.h"

#include "IM_FileSystem.h"
#include "IM_ModelImporter.h"

#include "MO_Scene.h"
#include "MO_ResourceManager.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/vector3.h"

#pragma comment (lib, "Assimp/libx86/assimp-vc142-mt.lib")


void  AnimationLoader::logCallback(const char* message, char* user)
{
	EngineExternal->moduleEditor->LogToConsole(message);
}

ResourceAnimation* AnimationLoader::LoadAnimation(aiAnimation* importedAnimation, uint oldUID)
{
	uint UID = oldUID;
	if (UID == 0)
		UID = EngineExternal->moduleResources->GenerateNewUID();

	LOG(LogType::L_NORMAL, "%s", importedAnimation->mName.C_Str());
	std::string file = ANIMATIONS_PATH;
	file += std::to_string(UID);
	file += ".anim";

	ResourceAnimation* anim = dynamic_cast<ResourceAnimation*>(EngineExternal->moduleResources->CreateNewResource("", UID, Resource::Type::ANIMATION));

	anim->ticksPerSecond = importedAnimation->mTicksPerSecond;
	anim->duration = importedAnimation->mDuration;

	for (int i = 0; i < importedAnimation->mNumChannels; i++)
	{
		Channel channel;
		channel.boneName = importedAnimation->mChannels[i]->mNodeName.C_Str();

		for (int j = 0; j < importedAnimation->mChannels[i]->mNumPositionKeys; j++)
		{
			aiVector3t<float> aiValue = importedAnimation->mChannels[i]->mPositionKeys[j].mValue;
			float3 positionKey = float3(aiValue.x, aiValue.y, aiValue.z);

			channel.positionKeys[importedAnimation->mChannels[i]->mPositionKeys[j].mTime] = positionKey;
		}

		for (int j = 0; j < importedAnimation->mChannels[i]->mNumRotationKeys; j++)
		{
			aiQuaterniont<float> aiValue = importedAnimation->mChannels[i]->mRotationKeys[j].mValue;
			Quat rotationKey = Quat(aiValue.x, aiValue.y, aiValue.z, aiValue.w);

			channel.rotationKeys[importedAnimation->mChannels[i]->mRotationKeys[j].mTime] = rotationKey;
		}

		for (int j = 0; j < importedAnimation->mChannels[i]->mNumScalingKeys; j++)
		{
			aiVector3t<float> aiValue = importedAnimation->mChannels[i]->mScalingKeys[j].mValue;
			float3 scaleKey = float3(aiValue.x, aiValue.y, aiValue.z);

			channel.scaleKeys[importedAnimation->mChannels[i]->mScalingKeys[j].mTime] = scaleKey;
		}
		anim->channels[channel.boneName] = channel;
	}
	//Save animation own format
	char* buffer;
	uint size = anim->SaveCustomFormat(anim, &buffer);
	
	FileSystem::Save(file.c_str(), buffer, size, false);
	RELEASE_ARRAY(buffer);

	return anim;
}

uint AnimationLoader::GetChannelsSize(const Channel& channel)
{
	uint ret = sizeof(uint) + channel.boneName.size() + sizeof(uint) * 3;
	//Translation
	ret += sizeof(double) * channel.positionKeys.size() + sizeof(float) * channel.positionKeys.size() * 3;
	//Rotation
	ret += sizeof(double) * channel.rotationKeys.size() + sizeof(float) * channel.rotationKeys.size() * 4;
	//Scale
	ret += sizeof(double) * channel.scaleKeys.size() + sizeof(float) * channel.scaleKeys.size() * 3;

	return ret;
}

void AnimationLoader::SaveChannels(const Channel& channel, char** cursor)
{
	//Name (size and string)
	uint nameSize = channel.boneName.size();
	memcpy(*cursor, &nameSize, sizeof(uint));
	*cursor += sizeof(uint);

	memcpy(*cursor, channel.boneName.c_str(), channel.boneName.size());
	*cursor += channel.boneName.size();

	//Ranges
	uint ranges[3] = { channel.positionKeys.size(), channel.rotationKeys.size(), channel.scaleKeys.size() };
	memcpy(*cursor, ranges, sizeof(uint) * 3);
	*cursor += sizeof(uint) * 3;

	SaveChannelKeys(channel.positionKeys, cursor);
	SaveChannelKeys(channel.rotationKeys, cursor);
	SaveChannelKeys(channel.scaleKeys, cursor);
}

void AnimationLoader::SaveChannelKeys(const std::map<double, float3>& map, char** cursor)
{
	std::map<double, float3>::const_iterator it = map.begin();
	for (it = map.begin(); it != map.end(); it++)
	{
		memcpy(*cursor, &it->first, sizeof(double));
		*cursor += sizeof(double);

		memcpy(*cursor, &it->second, sizeof(float) * 3);
		*cursor += sizeof(float) * 3;
	}
}

void AnimationLoader::SaveChannelKeys(const std::map<double, Quat>& map, char** cursor)
{
	std::map<double, Quat>::const_iterator it = map.begin();
	for (it = map.begin(); it != map.end(); it++)
	{
		memcpy(*cursor, &it->first, sizeof(double));
		*cursor += sizeof(double);

		memcpy(*cursor, &it->second, sizeof(float) * 4);
		*cursor += sizeof(float) * 4;
	}
}
