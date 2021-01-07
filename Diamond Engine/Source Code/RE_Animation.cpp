#include "Application.h"
#include "DEResource.h"
#include "IM_FileSystem.h"
#include "MO_ResourceManager.h"
#include "RE_Animation.h"
#include "IM_AnimationImporter.h"

ResourceAnimation::ResourceAnimation(unsigned int _uid) : Resource(_uid, Resource::Type::ANIMATION),duration(0),ticksPerSecond(0),numChannels(0),loopable(true)
{
}

ResourceAnimation::~ResourceAnimation()
{
}

bool ResourceAnimation::LoadToMemory()
{
	LOG(LogType::L_WARNING, "Animation loaded to memory");

	LoadCustomFormat(GetLibraryPath());

	return true;
}

bool ResourceAnimation::UnloadFromMemory()
{
	return false;
}

uint ResourceAnimation::SaveCustomFormat(ResourceAnimation* animation, char** buffer)
{
	uint size = sizeof(float) + sizeof(float) + sizeof(uint);

	//Channels size 
	std::map<std::string, Channel>::const_iterator it;
	for (it = animation->channels.begin(); it != animation->channels.end(); ++it)
		size += AnimationLoader::GetChannelsSize(it->second);

	//Allocate buffer size
	*buffer = new char[size];
	char* cursor = *buffer;

	//Duration
	memcpy(cursor, &animation->duration, sizeof(float));
	cursor += sizeof(float);

	//Ticks per second
	memcpy(cursor, &animation->ticksPerSecond, sizeof(float));
	cursor += sizeof(float);

	//Channels number
	uint channelsSize = animation->channels.size();
	memcpy(cursor, &channelsSize, sizeof(uint));
	cursor += sizeof(uint);

	//Save each channel data
	for (it = animation->channels.begin(); it != animation->channels.end(); ++it)
		AnimationLoader::SaveChannels(it->second, &cursor);

	return size;
}

void ResourceAnimation::LoadCustomFormat(const char* path)
{
	char* buffer = nullptr;

	uint size = FileSystem::LoadToBuffer(path, &buffer);

	const char* cursor = buffer;
	uint bytes;

	//Fills Duration
	memcpy(&duration, cursor, sizeof(float));
	cursor += sizeof(float);

	//Fills Ticks per second
	memcpy(&ticksPerSecond, cursor, sizeof(float));
	cursor += sizeof(float);

	//Fills Channels number
	uint channelsSize = 0;
	memcpy(&channelsSize, cursor, sizeof(uint));
	cursor += sizeof(uint);

	//Fills channels
	for (uint i = 0; i < channelsSize; ++i)
	{
		Channel Channelat;
		AnimationLoader::LoadChannels(Channelat, &cursor);
		channels[Channelat.boneName.c_str()] = Channelat;
	}
}
