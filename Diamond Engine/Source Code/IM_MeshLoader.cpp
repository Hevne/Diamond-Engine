#include "IM_MeshLoader.h"
#include "Application.h"
#include "MO_Editor.h"

#include "GameObject.h"
#include "CO_Transform.h"
#include "CO_MeshRenderer.h"
#include "CO_Material.h"
#include "CO_Animator.h"

#include "RE_Mesh.h"
#include "RE_Texture.h"

#include "IM_FileSystem.h"
#include "IM_TextureImporter.h"
#include "IM_ModelImporter.h"

#include "MO_Scene.h"
#include "MO_ResourceManager.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp-vc142-mt.lib")


void  MeshLoader::logCallback(const char* message, char* user)
{
	EngineExternal->moduleEditor->LogToConsole(message);
}

void MeshLoader::EnableDebugMode()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = logCallback;
	aiAttachLogStream(&stream);
}

void MeshLoader::DisableDebugMode()
{
	// detach log stream
	aiDetachAllLogStreams();
}

//Following unity tree structure, comments represent blender tree structure
void MeshLoader::NodeToGameObject(aiMesh** meshArray, std::vector<ResourceTexture*>& sceneTextures, std::vector<ResourceMesh*>& _sceneMeshes, aiNode* node, GameObject* gmParent, const char* holderName)
{

	aiVector3D		aiTranslation, aiScale;
	aiQuaternion	aiRotation;

	//Decomposing transform matrix into translation rotation and scale
	node->mTransformation.Decompose(aiScale, aiRotation, aiTranslation);

	float3 pos(aiTranslation.x, aiTranslation.y, aiTranslation.z);
	float3 scale(aiScale.x, aiScale.y, aiScale.z);
	Quat rot(aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w);

	std::string nodeName = node->mName.C_Str();
	bool dummyFound = true;
	while (dummyFound)
	{
		dummyFound = false;

		//All dummy modules have one children (next dummy module or last module containing the mesh)
		if (nodeName.find("_$AssimpFbx$_") != std::string::npos && node->mNumChildren == 1)
		{
			//Dummy module have only one child node, so we use that one as our next node
			node = node->mChildren[0];

			// Accumulate transform 
			node->mTransformation.Decompose(aiScale, aiRotation, aiTranslation);
			pos += float3(aiTranslation.x, aiTranslation.y, aiTranslation.z);
			scale = float3(scale.x * aiScale.x, scale.y * aiScale.y, scale.z * aiScale.z);
			rot = rot * Quat(aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w);

			nodeName = node->mName.C_Str();
			dummyFound = true;
		}
	}

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		ResourceMesh* meshPointer = _sceneMeshes[node->mMeshes[i]];

		GameObject* gmNode = new GameObject(node->mName.C_Str(), gmParent); //Name should be scene->mMeshes[node->mMeshes[i]]

		//Load mesh to GameObject
		C_MeshRenderer* gmMeshRenderer = dynamic_cast<C_MeshRenderer*>(gmNode->AddComponent(Component::Type::MeshRenderer));

		gmMeshRenderer->SetRenderMesh(meshPointer);

		aiMesh* importedMesh = meshArray[node->mMeshes[i]];
		if (importedMesh->mMaterialIndex < sceneTextures.size() && sceneTextures[importedMesh->mMaterialIndex] != nullptr)
		{
			C_Material* material = dynamic_cast<C_Material*>(gmNode->AddComponent(Component::Type::Material));
			material->matTexture = sceneTextures[importedMesh->mMaterialIndex];
		}
		PopulateTransform(gmNode, pos, rot, scale);
	}

	if (node->mNumChildren > 0)
	{
		GameObject* rootGO = gmParent;

		//WARNING: This could break the code if the gameobject that we are ignoring has some other components
		if (node->mNumChildren == 1 && node->mParent == nullptr && node->mChildren[0]->mNumChildren == 0) 
		{
			LOG(LogType::L_WARNING, "This is a 1 child gameObject, you could ignore the root node parent creation");
			node->mChildren[0]->mName = holderName;
		}
		else
		{
			rootGO = new GameObject(nodeName.c_str(), gmParent);
			PopulateTransform(rootGO, pos, rot, scale);
		}

		
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			NodeToGameObject(meshArray, sceneTextures, _sceneMeshes, node->mChildren[i], rootGO, node->mChildren[i]->mName.C_Str());
		}
	}
}


ResourceMesh* MeshLoader::LoadMesh(aiMesh* importedMesh, uint oldUID)
{
	uint UID = oldUID;
	if (UID == 0)
		UID = EngineExternal->moduleResources->GenerateNewUID();

	LOG(LogType::L_NORMAL, "%s", importedMesh->mName.C_Str());
	std::string file = MESHES_PATH;
	file += std::to_string(UID);
	file += ".mmh";

	ResourceMesh* _mesh = dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->CreateNewResource("", UID, Resource::Type::MESH));

	// copy vertices
	_mesh->vertices_count = importedMesh->mNumVertices;
	_mesh->vertices = new float[_mesh->vertices_count * 3];

	memcpy(_mesh->vertices, importedMesh->mVertices, sizeof(float) * _mesh->vertices_count * 3);

	LOG(LogType::L_NORMAL, "New mesh with %d vertices", _mesh->vertices_count);

	if (importedMesh->HasNormals())
	{
		_mesh->normals_count = _mesh->vertices_count;

		_mesh->normals = new float[_mesh->normals_count * 3];
		memcpy(_mesh->normals, importedMesh->mNormals, sizeof(float) * _mesh->normals_count * 3);

		LOG(LogType::L_NORMAL, "New mesh with %d normals", _mesh->normals_count);
	}

	//So are we really only supporting 1 channel uv and colors?
	if (importedMesh->HasTextureCoords(0))
	{
		_mesh->texCoords_count = importedMesh->mNumVertices;
		_mesh->texCoords = new float[importedMesh->mNumVertices * 2];

		for (unsigned int k = 0; k < _mesh->texCoords_count; k++)
		{
			_mesh->texCoords[k * 2] = importedMesh->mTextureCoords[0][k].x;
			_mesh->texCoords[k * 2 + 1] = importedMesh->mTextureCoords[0][k].y;
		}
		//_mesh->textureID = temporalTexID;

		LOG(LogType::L_NORMAL, "New mesh with %d texture coords", _mesh->texCoords_count);
	}

	//TODO: Load vertex colors
	if (importedMesh->HasVertexColors(0)) 
	{
		LOG(LogType::L_ERROR, "ADD VERTEX COLORS");
	}


	// Generate indices
	if (importedMesh->HasFaces())
	{
		_mesh->indices_count = importedMesh->mNumFaces * 3;
		_mesh->indices = new uint[_mesh->indices_count]; // assume each face is a triangle
		for (uint j = 0; j < importedMesh->mNumFaces; ++j)
		{
			if (importedMesh->mFaces[j].mNumIndices != 3)
			{
				LOG(LogType::L_WARNING, "WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&_mesh->indices[j * 3], importedMesh->mFaces[j].mIndices, 3 * sizeof(uint));
			}
		}
	}

	if (importedMesh->HasBones())
	{
		_mesh->hasSkeleton = true;
		LoadBones(importedMesh, _mesh);
	}

	_mesh->generalWireframe = &EngineExternal->moduleRenderer3D->wireframe;

	//TODO: Save on own file format
	uint size = 0;
	char* buffer;
	if (_mesh->hasSkeleton)
	{
		buffer = (char*)_mesh->SaveCustomFormatwithBones(size);
	}
	else
	{
		buffer = (char*)_mesh->SaveCustomFormat(size);
	}
	FileSystem::Save(file.c_str(), buffer, size, false);
	RELEASE_ARRAY(buffer);

	return _mesh;
}

void MeshLoader::LoadBones(const aiMesh* importedMesh, ResourceMesh* ourMesh)
{
	ourMesh->bonesTransforms.resize(importedMesh->mNumBones);
	
	//Set the bones array each 4
	ourMesh->bones_count = importedMesh->mNumVertices * 4;
	ourMesh->bones = new int[ourMesh->bones_count];
	for (int i = 0; i < ourMesh->bones_count; i++)
	{
		ourMesh->bones[i] = -1;
	}

	//Set the weights array each 4
	ourMesh->weights_count = importedMesh->mNumVertices * 4;
	ourMesh->boneWeights = new float[ourMesh->weights_count];
	for (int i = 0; i < ourMesh->weights_count; i++)
	{
		ourMesh->boneWeights[i] = 0.f;
	}

	for (int boneIndex = 0; boneIndex < importedMesh->mNumBones; boneIndex++)
	{
		aiBone* bone = importedMesh->mBones[boneIndex];
		ourMesh->bonesMap[bone->mName.C_Str()] = boneIndex;

		//Load offsets
		float4x4 offset = float4x4(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.a2, bone->mOffsetMatrix.a3, bone->mOffsetMatrix.a4,
				bone->mOffsetMatrix.b1, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.b4,
				bone->mOffsetMatrix.c1, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.c4,
				bone->mOffsetMatrix.d1, bone->mOffsetMatrix.d2, bone->mOffsetMatrix.d3, bone->mOffsetMatrix.d4);

		ourMesh->bonesOffsets.push_back(offset);

		//Iterates all affected mesh vertices
		for (int i = 0; i < bone->mNumWeights; i++)
		{
			uint index = bone->mWeights[i].mVertexId;
			//Fills each bone or weight array empty slot (-1)
			for (int j = 0; j < 4; ++j)
			{
				if (ourMesh->bones[index * 4 + j] == -1)
				{
					ourMesh->bones[index * 4 + j] = boneIndex;
					ourMesh->boneWeights[index + 4 + j] = bone->mWeights[i].mWeight;
				}
			}
		}

	}

}

void MeshLoader::PopulateTransform(GameObject* child, float3 position, Quat rotationQuat, float3 size)
{
	child->transform->SetTransformMatrix(position, rotationQuat, size);
}