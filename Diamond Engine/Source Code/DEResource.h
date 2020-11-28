#pragma once

#include<string>

class Resource
{

public:

	//REMEBER: To update all the switch methods on Resource Manager module when adding a new type
	enum class Type {
		TEXTURE,
		MODEL,
		MESH,
		//AUDIO,
		SCENE,
		//BONE,
		//ANIMATION,
		UNKNOWN
	};

public:
	Resource(int _uid, Resource::Type _type);
	virtual ~Resource();

	inline Resource::Type GetType() const { return type; }
	inline int GetUID() const { return uid; }
	inline const char* GetAssetPath() const { return assetsFile.c_str(); }
	inline const char* GetLibraryPath() const { return libraryFile.c_str(); }

	void SetAssetsPath(const char*);
	void SetLibraryPath(const char*);

	//inline bool IsLoadedToMemory() const { return (referenceCount >= 0) ? true : false; }

	inline unsigned int GetReferenceCount() const { return referenceCount; }
	inline void IncreaseReferenceCount() { referenceCount++; }
	inline void DecreaseReferenceCount() { referenceCount--; }

	//virtual void Save(Config& config) const;
	//virtual void Load(const Config& config);

	//virtual bool LoadToMemory() = 0;
	virtual bool LoadToMemory() = 0;
	virtual bool UnloadFromMemory() = 0;

protected:

	int uid;
	std::string assetsFile;
	std::string libraryFile;
	Type type = Type::UNKNOWN;
	unsigned int referenceCount;
};