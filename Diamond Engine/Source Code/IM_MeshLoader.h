#pragma once

#include <vector>
#include"MathGeoLib/include/Math/Quat.h"

class ResourceMesh;
class aiNode;
class aiMesh;
class aiAnimation;
class GameObject;
class ResourceTexture;
class ResourceAnimation;
typedef unsigned int uint;

//Change this to file system
namespace MeshLoader
{
	void EnableDebugMode();
	void DisableDebugMode();

	void logCallback(const char* message, char* user);

	void NodeToGameObject(aiMesh** meshArray, std::vector<ResourceTexture*>& sceneTextures, std::vector<ResourceMesh*>& _sceneMeshes, aiNode* node, GameObject* gmParent, const char* holderName);
	ResourceMesh* LoadMesh(aiMesh* importedMesh, uint oldUID = 0);
	void LoadBones(const aiMesh* importedMesh, ResourceMesh* ourMesh);

	void PopulateTransform(GameObject* child, float3 position, Quat rotationQuat, float3 size);
}
