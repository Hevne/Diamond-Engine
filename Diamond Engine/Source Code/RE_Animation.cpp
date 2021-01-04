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
	return false;
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

	//Ticks per sec
	memcpy(cursor, &animation->ticksPerSecond, sizeof(float));
	cursor += sizeof(float);

	//Channels number
	uint channelsSize = animation->channels.size();
	memcpy(cursor, &channelsSize, sizeof(uint));
	cursor += sizeof(uint);

	for (it = animation->channels.begin(); it != animation->channels.end(); ++it)
		AnimationLoader::SaveChannels(it->second, &cursor);

	return size;

}
