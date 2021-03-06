#include "CO_MeshRenderer.h"
#include "RE_Mesh.h"
#include "OpenGL.h"

#include "Application.h"
#include "MO_Renderer3D.h"
#include "IM_FileSystem.h"
#include"MO_ResourceManager.h"

#include "GameObject.h"
#include "CO_Material.h"
#include "CO_Transform.h"
#include"CO_Camera.h"
#include "CO_Animator.h"

#include "ImGui/imgui.h"
#include"DEJsonSupport.h"

#include"MathGeoLib/include/Geometry/Frustum.h"
#include"MathGeoLib/include/Geometry/Plane.h"

C_MeshRenderer::C_MeshRenderer(GameObject* _gm) : Component(_gm), _mesh(nullptr),
faceNormals(false), vertexNormals(false), showAABB(true), showOBB(true)
{
	name = "Mesh Renderer";
}

C_MeshRenderer::~C_MeshRenderer()
{
	if (_mesh != nullptr) 
	{
		EngineExternal->moduleResources->UnloadResource(_mesh->GetUID());
		_mesh = nullptr;
	}
}

void C_MeshRenderer::Update(float dt)
{

	if (EngineExternal->moduleRenderer3D->GetGameRenderTarget() != nullptr && !IsInsideFrustum(&EngineExternal->moduleRenderer3D->GetGameRenderTarget()->camFrustrum))
		return;
	
	EngineExternal->moduleRenderer3D->renderQueue.push_back(this);

	if (showAABB ==true) {
		float3 points[8];
		globalAABB.GetCornerPoints(points);
		ModuleRenderer3D::DrawBox(points, float3(0.2f, 1.f, 0.101f));
	}

	if (showOBB == true) {
		float3 points[8];
		globalOBB.GetCornerPoints(points);
		ModuleRenderer3D::DrawBox(points);
	}
}

void C_MeshRenderer::RenderMesh()
{
	if (_mesh == nullptr)
		return;


	//Position matrix?
	//BUG: Mesh rendering will crash if you dont fix the resource manager for meshes
	C_Transform* transform = gameObject->transform;
	if (transform != nullptr)
	{
		glPushMatrix();
		glMultMatrixf(transform->GetGlobalTransposed());
	}

	C_Material* material = dynamic_cast<C_Material*>(gameObject->GetComponent(Component::Type::Material));
	GLuint id = 0;

	if (material != nullptr && material->IsActive())
		id = material->GetTextureID();

	if (_animableMesh != nullptr && dynamic_cast<C_Animator*>(gameObject->parent->GetComponent(Component::Type::Animator))->playing)
	{
		 _animableMesh->RenderMesh(id);
		 //for (uint v = 0; v < _animableMesh->vertices_count; v++)
		 //{
			// float3 vertexV(_animableMesh->vertices[v * 3], _animableMesh->vertices[v * 3 + 1], _animableMesh->vertices[v * 3 + 2]);
			// glColor3f(1.f, 0.f, 1.f);
			// glPointSize(20.f);
			// glBegin(GL_POINTS);
			// glVertex3f(vertexV.x, vertexV.y, vertexV.z);
			// glEnd();
			// glPointSize(1.f);
			// glColor3f(1.f, 1.f, 1.f);
		 //}
		 
	}
	else
	{
		_mesh->RenderMesh(id);
		//for (uint v = 0; v < _mesh->vertices_count; v++)
		//{
		//	float3 vertexV(_mesh->vertices[v * 3], _mesh->vertices[v * 3 + 1], _mesh->vertices[v * 3 + 2]);
		//	glColor3f(1.f, 0.f, 1.f);
		//	glPointSize(20.f);
		//	glBegin(GL_POINTS);
		//	glVertex3f(vertexV.x, vertexV.y, vertexV.z);
		//	glEnd();
		//	glPointSize(1.f);
		//	glColor3f(1.f, 1.f, 1.f);
		//}
	}

	

	if (vertexNormals || faceNormals)
	{
		if (_animableMesh != nullptr)
		{
			_animableMesh->RenderMeshDebug(&vertexNormals, &faceNormals);
		}
		else
		{
			_mesh->RenderMeshDebug(&vertexNormals, &faceNormals);
		}
	}

	if (transform != nullptr)
		glPopMatrix();
}

void C_MeshRenderer::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);
	DEJson::WriteString(nObj, "Path", _mesh->GetLibraryPath());
	DEJson::WriteInt(nObj, "UID", _mesh->GetUID());
}
void C_MeshRenderer::LoadData(JSON_Object* nObj)
{
	Component::LoadData(nObj);


	//There is no _mesh yet lol
	DEConfig jsObj(nObj);

	SetRenderMesh(dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->RequestResource(jsObj.ReadInt("UID"), jsObj.ReadString("Path"))));

	if (_mesh == nullptr)
		return;

	_mesh->generalWireframe = &EngineExternal->moduleRenderer3D->wireframe;

	gameObject->transform->UpdateBoxes();
}

bool C_MeshRenderer::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		if (_mesh != nullptr) 
		{
			//ImGui::Image((ImTextureID)_mesh->textureID, ImVec2(128, 128));
			ImGui::Text("Vertices: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->vertices_count);
			ImGui::Text("Normals: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->normals_count);
			ImGui::Text("Indices: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->indices_count);
			ImGui::Text("Texture coords: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->texCoords_count);

			ImGui::Spacing();
			ImGui::Text("Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", _mesh->GetLibraryPath());
		}

		ImGui::Button("Drop .mmh to change mesh", ImVec2(200, 50));
		//TODO: Maybe move this into a function?
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MESH"))
			{
				std::string* libraryDrop = (std::string*)payload->Data;

				if (_mesh != nullptr) {
					EngineExternal->moduleResources->UnloadResource(_mesh->GetUID());
					_mesh = nullptr;
				}

				//TODO: Almost done come on TEMPORAL: This is the only way until fbx's .meta files
				//Store an array of meshes to reference
				std::string stID = "";
				FileSystem::GetFileName(libraryDrop->c_str(), stID, false);

				//ATOI is C++11 only, maybe not a good idea to use it
				int UID = std::atoi(stID.c_str());
				SetRenderMesh(dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->RequestResource(UID, libraryDrop->c_str())));
				LOG(LogType::L_WARNING, "Mesh %s changed", (*libraryDrop).c_str());
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Checkbox("Vertex Normals", &vertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &faceNormals);

		ImGui::Checkbox("Show AABB", &showAABB);
		ImGui::SameLine();
		ImGui::Checkbox("Show OBB", &showOBB);

		return true;
	}
	return false;
}

bool C_MeshRenderer::IsInsideFrustum(Frustum* camFrustum)
{
	float3 obbPoints[8];
	Plane frustumPlanes[6];

	int totalIn = 0;

	globalAABB.GetCornerPoints(obbPoints);
	camFrustum->GetPlanes(frustumPlanes);

	for (size_t i = 0; i < 6; i++)
	{
		int inCount = 8;
		int iPtIn = 1;

		for (size_t k = 0; k < 8; k++)
		{
			//Is "IsOnPositiveSide" slow?
			if (frustumPlanes[i].IsOnPositiveSide(obbPoints[k])) 
			{
				iPtIn = 0;
				--inCount;
			}
			if (inCount == 0)
				return false;

			totalIn += iPtIn;
		}
	}

	if (totalIn == 6)
		return true;

	return true;
}

void C_MeshRenderer::SetRenderMesh(ResourceMesh* mesh)
{ 
	_mesh = mesh;
	//_mesh->LoadCustomFormat(_mesh->GetLibraryPath());

	if (mesh == nullptr)
		return;

	globalOBB = _mesh->localAABB;
	globalOBB.Transform(gameObject->transform->globalTransform);

	// Generate global AABB
	globalAABB.SetNegativeInfinity();
	globalAABB.Enclose(globalOBB);
}

ResourceMesh* C_MeshRenderer::GetRenderMesh()
{
	return _mesh;
}

ResourceMesh* C_MeshRenderer::GetRenderAnimableMesh()
{
	return _animableMesh;
}

void C_MeshRenderer::DuplicateMeshintoAnimable()
{
	bool duplicateMesh = false;
	if (_animableMesh == nullptr)
	{
		duplicateMesh = true;
		_animableMesh = new ResourceMesh(0); 
		//Get the current mesh
		ResourceMesh* rMesh = gameObject->GetMesh()->_mesh; 

		//Copy buffers sizes
		_animableMesh->indices_count = rMesh->indices_count;
		_animableMesh->vertices_count = rMesh->vertices_count;
		_animableMesh->normals_count = rMesh->normals_count;
		_animableMesh->texCoords_count = rMesh->texCoords_count;

		//To render the new texture
		_animableMesh->generalWireframe = rMesh->generalWireframe;
		
		//Create buffer arrays
		_animableMesh->indices = new uint[rMesh->indices_count];
		_animableMesh->vertices = new float[rMesh->vertices_count * 3];
		_animableMesh->normals = new float[rMesh->normals_count * 3];
		_animableMesh->texCoords = new float[rMesh->texCoords_count * 2];

		//Storage indices and texCoords
		memcpy(_animableMesh->indices, rMesh->indices, rMesh->indices_count * sizeof(uint));
		memcpy(_animableMesh->texCoords, rMesh->texCoords, rMesh->texCoords_count * 2 * sizeof(float));
	}

	//Initialize at 0 vertices, normals and texCoords for future storage after Delta calcule
	memset(_animableMesh->vertices, 0, _animableMesh->vertices_count * sizeof(float) * 3);

	if (_animableMesh->normals_count > 0)
	{
		memset(_animableMesh->normals, 0, _animableMesh->normals_count * sizeof(float) * 3);
	}

	if (duplicateMesh) {
		_animableMesh->RegenerateBuffers(true); // Load buffers eudald
	}
}

void C_MeshRenderer::MoveVerticesnNormals()
{
	//Get the root GObject mesh
	ResourceMesh* rMesh = gameObject->GetMesh()->_mesh;

	//Get all the bones
	std::map<std::string, GameObject*> bonesMap;
	GetBoneMapping(bonesMap);

	//Set bone Transforms array size using original bones transform array size
	std::vector<float4x4> boneTransforms;
	boneTransforms.resize(rMesh->bonesOffsets.size());

	//Get each bone
	for (std::map<std::string, uint>::iterator it = rMesh->bonesMap.begin(); it != rMesh->bonesMap.end(); ++it)
	{
		GameObject* bone = bonesMap[it->first];

		if (bone != nullptr)
		{
			//Calcule of Delta Matrix
			float4x4 Delta = CalculateDeltaMatrix(dynamic_cast<C_Transform*>(bone->GetComponent(Component::Type::Transform))->GetGlobalMatrix(), dynamic_cast<C_Transform*>(gameObject->GetComponent(Component::Type::Transform))->GetGlobalMatrix().Inverted());
			Delta = Delta * rMesh->bonesOffsets[it->second];

			//Storage of Delta Matrix (Transformation applied to each bone)
			boneTransforms[it->second] = Delta;
		}
	}

	//For all mesh vertices
	for (uint v = 0; v < rMesh->vertices_count; ++v)
	{
		//For each set of 4 bones for bertex
		for (uint b = 0; b < 4; ++b)
		{
			//Get bone identificator and weights from arrays
			int bone_ID = rMesh->bones[v * 4 + b];
			float boneWeight = rMesh->boneWeights[v * 4 + b];

			//Meaning boneWeight will be 0
			if (bone_ID == -1) continue;

			//Transforming original mesh vertex with bone transformation matrix
			float3 Inc = boneTransforms[bone_ID].TransformPos(float3(&rMesh->vertices[v * 3]));
			//float3 vertexV(_animableMesh->vertices[v * 3], _animableMesh->vertices[v * 3 + 1], _animableMesh->vertices[v * 3 + 2]);

			_animableMesh->vertices[v * 3] += Inc.x * boneWeight;
			_animableMesh->vertices[v * 3 + 1] += Inc.y * boneWeight;
			_animableMesh->vertices[v * 3 + 2] += Inc.z * boneWeight;

			//float3 vertexF(_animableMesh->vertices[v * 3], _animableMesh->vertices[v * 3 + 1], _animableMesh->vertices[v * 3 + 2]);

			if (rMesh->normals_count > 0)
			{
				//Transforming original mesh normal with bone transformation matrix
				Inc = boneTransforms[bone_ID].TransformPos(float3(&rMesh->normals[v * 3]));
				_animableMesh->normals[v * 3] += Inc.x * boneWeight;
				_animableMesh->normals[v * 3 + 1] += Inc.y * boneWeight;
				_animableMesh->normals[v * 3 + 2] += Inc.z * boneWeight;
			}
		}
	}

	_animableMesh->RegenerateBuffers();
}

float4x4 C_MeshRenderer::CalculateDeltaMatrix(float4x4 globalMat, float4x4 invertMat)
{
	float3 position;
	Quat rotation;
	float3 scale;

	float4x4 mat = globalMat;
	mat.Decompose(position, rotation, scale);
	mat = dynamic_cast<C_Transform*>(gameObject->GetComponent(Component::Type::Transform))->GetGlobalMatrix().Inverted()* mat;
	mat.Decompose(position, rotation, scale);

	return mat;
}

void C_MeshRenderer::GetBoneMapping(std::map<std::string, GameObject*>& boneMapping)
{
	boneMapping.clear();
	std::vector<GameObject*> gameObjects;
	rootBone->CollectChilds(gameObjects);

	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		boneMapping[gameObjects[i]->name] = gameObjects[i];
	}
}
