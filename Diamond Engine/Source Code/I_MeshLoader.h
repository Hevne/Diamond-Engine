#pragma once

#include <vector>
#include"MathGeoLib/include/Math/Quat.h"

class Mesh;
class aiNode;
class aiMesh;
class GameObject;
class Texture;

//Change this to file system
namespace MeshLoader
{
	void EnableDebugMode();
	void DisableDebugMode();

	void logCallback(const char* message, char* user);

	void BufferToMeshes(const char* full_path, char* buffer, int size, GameObject* gmRoot);

	void NodeToGameObject(aiMesh** meshArray, std::vector<Texture*>& sceneTextures, std::vector<Mesh*>& _sceneMeshes, aiNode* node, GameObject* gmParent, const char* holderName);
	Mesh* LoadMesh(aiMesh* importedMesh);

	void PopulateTransform(GameObject* child, float3 position, Quat rotationQuat, float3 size);

	//Custom mesh and image systems
	//void SaveCMesh(Mesh& _mesh);
	//Mesh* LoadCMesh();
}
