#pragma once

#include "glmath.h" //Move to MathGeoLib
#include <vector>
#include <map>
#include<string>

#include"MathGeoLib\include\MathGeoLib.h"
#include"DEResource.h"

typedef unsigned int GLuint;
typedef unsigned int uint;

class ResourceMesh  : public Resource
{

public:
	ResourceMesh(unsigned int _uid);
	~ResourceMesh();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

	void RenderMesh(GLuint textureID);
	void RenderMeshDebug(bool* vertexNormals, bool* faceNormals);

	vec3 GetVectorFromIndex(float* startValue);

public:

	uint indices_id = 0; // index in VRAM
	uint indices_count = 0;
	uint* indices = nullptr;

	//Vertices is a 1 dimension array that contains floats representing a vertex every 3 elements, vertices_count is the number of 3 pairs (0, 0, 1) would be
	//a 3 element array but vertices_count would be 1.
	uint vertices_id = 0; // unique vertex in VRAM
	uint vertices_count = 0;
	float* vertices = nullptr;

	uint normalbuffer_id = 0;
	uint normals_count = 0;
	float* normals = nullptr;

	uint texCoords_id = 0;
	uint texCoords_count = 0;
	float* texCoords = nullptr;

	//Bones stuff [Remember 4 max for vertices]
	bool hasSkeleton = false;

	uint bones_count = 0;
	int* bones = nullptr;

	uint weights_count = 0;
	float* boneWeights = nullptr;

	std::map<std::string, uint> bonesMap;
	std::vector<float4x4> bonesTransforms;
	std::vector<float4x4> bonesOffsets;

	AABB localAABB;

	//TODO: Delete this, wireframe mode should be different
	// ----------- TEMPORAL LOGIC, MUST BE DELETED ---------------//
	bool* generalWireframe;
	// ----------- ------------------------------- ---------------//

	void GenerateSphere(float radius, float sectorCount, float stackCount);

	//TODO: Move this to file system
	const char* SaveCustomFormatwithBones(uint& retSize);
	const char* SaveCustomFormat(uint& retSize);
	void SaveBones(char** cursor);
	void LoadCustomFormat(const char*);
	void LoadBones(char** cursor);

};