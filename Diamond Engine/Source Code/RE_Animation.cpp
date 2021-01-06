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

	ResourceAnimation* loaded_animation = AnimationLoader::LoadCustomFormat(GetLibraryPath());

	if (loaded_animation == nullptr)
		return false;
	else
		return true;
}

bool ResourceAnimation::UnloadFromMemory()
{
	return false;
}
