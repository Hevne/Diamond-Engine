#include "RE_Mesh.h"
#include "OpenGL.h"
#include "MeshArrays.h"
#include "IM_FileSystem.h"
#include"Globals.h"

ResourceMesh::ResourceMesh(unsigned int _uid) : Resource(_uid, Resource::Type::MESH), indices_id(0), vertices_id(0), generalWireframe(nullptr)
{

}

ResourceMesh::~ResourceMesh()
{
}

bool ResourceMesh::LoadToMemory()
{
	LOG(LogType::L_WARNING, "Mesh loaded to memory");
	LoadCustomFormat(GetLibraryPath());

	// vertices_count = vector3's // size of the array (elements) = vertices_count * 3 // size of the array in bytes = sizeof(float) * vertices_count * 3
	if (vertices_count != 0)
	{
		glGenBuffers(1, (GLuint*)&(vertices_id));
		glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_count * 3, &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glVertexPointer(3, GL_FLOAT, 0, NULL);
	}

	if (indices_count != 0)
	{
		// indices_count = elements // size of the array (elements) = indices_count // size of the array in bytes = sizeof(uint) * indices_count
		glGenBuffers(1, (GLuint*)&(indices_id));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices_count, &indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	if (normals_count != 0)
	{
		//Normals buffer
		glGenBuffers(1, (GLuint*)&(normalbuffer_id));
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals_count * 3, &normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	if (texCoords_count != 0)
	{
		//Normals buffer
		glGenBuffers(1, (GLuint*)&texCoords_id);
		glBindBuffer(GL_ARRAY_BUFFER, texCoords_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCoords_count * 2, texCoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	return true;
}

bool ResourceMesh::UnloadFromMemory()
{
	//Clear buffers (Should not happen on Mesh components)
	if (indices_id != 0)
		glDeleteBuffers(1, &indices_id);

	if (vertices_id != 0)
		glDeleteBuffers(1, &vertices_id);

	if (normalbuffer_id != 0)
		glDeleteBuffers(1, &normalbuffer_id);

	if (texCoords_id != 0)
		glDeleteBuffers(1, &texCoords_id);

	//Clear buffers
	if (indices != nullptr)
		delete[] indices;

	if (vertices != nullptr)
		delete[] vertices;

	if (normals != nullptr)
		delete[] normals;

	if (texCoords != nullptr)
		delete[] texCoords;

	//Restart to nullptr
	indices = nullptr;
	vertices = nullptr;
	normals = nullptr;
	texCoords = nullptr;

	return true;
}

void ResourceMesh::RenderMesh(GLuint textureID)
{
	//ASK: glDrawElementsInstanced()?
	if(textureID != 0 && (generalWireframe != nullptr && *generalWireframe == false))
		glBindTexture(GL_TEXTURE_2D, textureID);

	//Vertices --------------------------------------------
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	//--------------------------------------------


	//TexCoords --------------------------------------------
	if (texCoords_count != 0)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, texCoords_id);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//glEnableVertexAttribArray(1);
	}
	//--------------------------------------------

	//Normals --------------------------------------------
	if (normals_count != 0)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_id);
		glNormalPointer(GL_FLOAT, 0, NULL);
		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}
	//--------------------------------------------

	//TODO: Make a buffer for the colors and try this
	//glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	//glVertexAttribPointer((GLint)1, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);

	//Indices --------------------------------------------
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	//--------------------------------------------

	//Drawing --------------------------------------------
	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, NULL);
	//--------------------------------------------

	//Drawing cleanup --------------------------------------------
	if (textureID != 0 && (generalWireframe != nullptr && *generalWireframe == false))
		glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	if (normals_count != 0)
		glDisableClientState(GL_NORMAL_ARRAY);
	if (texCoords_count != 0)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//--------------------------------------------
}

void ResourceMesh::RenderMeshDebug(bool* vertexNormals, bool* faceNormals)
{
	if (*vertexNormals == true)
	{
		float normalLenght = 0.05f;
		glPointSize(3.0f);
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		for (unsigned int i = 0; i < vertices_count * 3; i += 3)
		{
			glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
		}
		glEnd();
		glColor3f(0, 1, 0);
		glPointSize(1.0f);

		//Vertex normals
		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < normals_count * 3; i += 3)
		{
			glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
			glVertex3f(vertices[i] + normals[i] * normalLenght, vertices[i + 1] + normals[i + 1] * normalLenght, vertices[i + 2] + normals[i + 2] * normalLenght);
		}
		glEnd();
		glColor3f(1, 1, 1);
	}

	if (*faceNormals == true)
	{
		float normalLenght = 0.05f;
		//Face normals
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		for (int i = 0; i < indices_count; i += 3)
		{
			vec3 A = GetVectorFromIndex(&vertices[indices[i] * 3]);
			vec3 B = GetVectorFromIndex(&vertices[indices[i + 1] * 3]);
			vec3 C = GetVectorFromIndex(&vertices[indices[i + 2] * 3]);

			vec3 middle((A.x + B.x + C.x) / 3.f, (A.y + B.y + C.y) / 3.f, (A.z + B.z + C.z) / 3.f);

			vec3 crossVec = cross((B - A), (C - A));
			vec3 normalDirection = normalize(crossVec);

			glVertex3f(middle.x, middle.y, middle.z);
			glVertex3f(middle.x + normalDirection.x * normalLenght, middle.y + normalDirection.y * normalLenght, middle.z + normalDirection.z * normalLenght);
		}
		glEnd();
		glPointSize(1.f);
		glColor3f(1, 1, 1);
	}
}

vec3 ResourceMesh::GetVectorFromIndex(float* startValue)
{
	float x = *startValue;
	++startValue;
	float y = *startValue;
	++startValue;
	float z = *startValue;

	return vec3(x, y, z);
}

#pragma region Sphere generation
void ResourceMesh::GenerateSphere(float radius, float sectorCount, float stackCount)
{
	// clear memory of prev arrays
	std::vector<float> _vertices;
	std::vector<float> _normals;
	std::vector<float> _texCoords;

	int selectorTracker = 0;

	vertices_count = normals_count = ((sectorCount + 1) * (stackCount + 1)); //Numero de elements, cal treure el *3?
	texCoords_count = ((sectorCount + 1) * (stackCount + 1)); //Numero de elements, cal treure el *2?
	indices_count = (((stackCount - 2) * (sectorCount * 2)) * 3) + ((2 * sectorCount) * 3);


	indices = new uint[indices_count];
	vertices = new float[vertices_count * 3];
	normals = new float[normals_count * 3];
	texCoords = new float[texCoords_count * 2];


	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * M_PI / sectorCount;
	float stackStep = M_PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = M_PI / 2.f - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			_vertices.push_back(x);
			_vertices.push_back(y);
			_vertices.push_back(z);

			//vertices[selectorTracker] = x;
			//selectorTracker++;
			//vertices[selectorTracker] = y;
			//selectorTracker++;
			//vertices[selectorTracker] = z;
			//selectorTracker++;


			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			_normals.push_back(nx);
			_normals.push_back(ny);
			_normals.push_back(nz);

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			_texCoords.push_back(s);
			_texCoords.push_back(t);
		}
	}

	// generate CCW index list of sphere triangles
	std::vector<uint> _indices;
	int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				_indices.push_back(k1);
				_indices.push_back(k2);
				_indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
			{
				_indices.push_back(k1 + 1);
				_indices.push_back(k2);
				_indices.push_back(k2 + 1);
			}
		}
	}

	memcpy(vertices, &_vertices[0], sizeof(float) * vertices_count * 3);
	memcpy(normals, &_normals[0], sizeof(float) * normals_count * 3);
	memcpy(texCoords, &_texCoords[0], sizeof(float) * texCoords_count * 2);
	memcpy(indices, &_indices[0], sizeof(uint) * indices_count);
}
#pragma endregion

const char* ResourceMesh::SaveCustomFormatwithBones(uint& retSize)
{
	uint aCounts[6] = { indices_count, vertices_count, normals_count, texCoords_count, bones_count, weights_count };
	retSize = sizeof(aCounts) + (sizeof(uint) * indices_count) + (sizeof(float) * vertices_count * 3) + (sizeof(float) * normals_count * 3) + (sizeof(float) * texCoords_count * 2)
		+ (sizeof(int) * bones_count) + (sizeof(float) * weights_count) + (sizeof(float) * 16 * bonesOffsets.size()) + (sizeof(char) * 30 * bonesMap.size());

	//Stores each bone size
	std::map<std::string, uint>::const_iterator it;
	for (it = bonesMap.begin(); it != bonesMap.end(); ++it)
	{
		retSize += sizeof(uint);
		retSize += sizeof(char) * it->first.size();
	}

	char* fileBuffer = new char[retSize];
	char* cursor = fileBuffer;

	uint bytes = sizeof(aCounts);
	memcpy(cursor, aCounts, bytes);
	cursor += bytes;

	bytes = sizeof(uint) * indices_count;
	memcpy(cursor, indices, bytes);
	cursor += bytes;

	bytes = sizeof(float) * vertices_count * 3;
	memcpy(cursor, vertices, bytes);
	cursor += bytes;

	bytes = sizeof(float) * normals_count * 3;
	memcpy(cursor, normals, bytes);
	cursor += bytes;

	bytes = sizeof(float) * texCoords_count * 2;
	memcpy(cursor, texCoords, bytes);
	cursor += bytes;

	SaveBones(&cursor);

	return fileBuffer; 
}

const char* ResourceMesh::SaveCustomFormat(uint& retSize)
{
	uint aCounts[4] = { indices_count, vertices_count, normals_count, texCoords_count};
	retSize = sizeof(aCounts) + (sizeof(uint) * indices_count) + (sizeof(float) * vertices_count * 3) + (sizeof(float) * normals_count * 3) + (sizeof(float) * texCoords_count * 2);

	char* fileBuffer = new char[retSize];
	char* cursor = fileBuffer;

	uint bytes = sizeof(aCounts);
	memcpy(cursor, aCounts, bytes);
	cursor += bytes;

	bytes = sizeof(uint) * indices_count;
	memcpy(cursor, indices, bytes);
	cursor += bytes;

	bytes = sizeof(float) * vertices_count * 3;
	memcpy(cursor, vertices, bytes);
	cursor += bytes;

	bytes = sizeof(float) * normals_count * 3;
	memcpy(cursor, normals, bytes);
	cursor += bytes;

	bytes = sizeof(float) * texCoords_count * 2;
	memcpy(cursor, texCoords, bytes);
	cursor += bytes;

	return fileBuffer;
}

void ResourceMesh::SaveBones(char** cursor)
{
	uint bytes = 0;
	
	if (bones_count > 0)
	{
		bytes = sizeof(int) * bones_count;
		memcpy(*cursor, bones, bytes);
		*cursor += bytes;
	}
	if (weights_count > 0)
	{
		bytes = sizeof(float) * weights_count;
		memcpy(*cursor, boneWeights, bytes);
		*cursor += bytes;
	}
	if (bonesOffsets.size() > 0)
	{
		for (int i = 0; i < bonesOffsets.size(); ++i)
		{
			bytes = sizeof(float) * 16;
			memcpy(*cursor, bonesOffsets[i].ptr(), bytes);
			*cursor += bytes;
		}
	}

	for (int i = 0; i < bonesMap.size(); ++i)
	{
		std::map<std::string, uint>::const_iterator it;
		for (it = bonesMap.begin(); it != bonesMap.end(); ++it)
		{
			if (it->second == i)
			{
				bytes = sizeof(uint);
				uint stringSize = it->first.size();
				memcpy(*cursor, &stringSize, bytes);
				*cursor += bytes;

				bytes = sizeof(char) * stringSize;
				memcpy(*cursor, it->first.c_str(), bytes);
				*cursor += bytes;
			}
		}
	}
}

void ResourceMesh::LoadCustomFormat(const char* path)
{
	char* fileBuffer = nullptr;

	uint size = FileSystem::LoadToBuffer(path, &fileBuffer);

	if (size == 0)
		return;

	char* cursor = fileBuffer;
	uint variables[4];

	uint bytes = sizeof(variables);
	memcpy(variables, cursor, bytes);
	indices_count = variables[0];
	vertices_count = variables[1];
	normals_count = variables[2];
	texCoords_count = variables[3];
	cursor += bytes;

	/*uint bones_variables[2];

	bytes = sizeof(bones_variables);
	memcpy(bones_variables, cursor, bytes);

	if (bones_variables[0] > 0)
	{
		bones_count = bones_variables[0];
		weights_count = bones_variables[1];
		cursor += bytes;

		bytes = sizeof(uint);
		uint bonesSize = 0;
		memcpy(&bonesSize, cursor, bytes);
		cursor += bytes;

		bonesTransforms.resize(bonesSize);
		bonesOffsets.resize(bonesSize);
	}*/




	bytes = sizeof(uint) * indices_count;

	indices = new uint[indices_count];
	memcpy(indices, cursor, bytes);
	cursor += bytes;

	vertices = new float[vertices_count * 3];
	bytes = sizeof(float) * vertices_count * 3;
	memcpy(vertices, cursor, bytes);
	cursor += bytes;

	normals = new float[normals_count * 3];
	bytes = sizeof(float) * normals_count * 3;
	memcpy(normals, cursor, bytes);
	cursor += bytes;

	texCoords = new float[texCoords_count * 2];
	bytes = sizeof(float) * texCoords_count * 2;
	memcpy(texCoords, cursor, bytes);
	cursor += bytes;
	
	LoadBones(&cursor);

	//TODO: Should this be here?
	localAABB.SetNegativeInfinity();
	localAABB.Enclose((float3*)vertices, vertices_count);

	delete[] fileBuffer;
	fileBuffer = nullptr;
}

void ResourceMesh::LoadBones(char** cursor)
{
	if (bones_count > 0)
	{

	}
	if (weights_count > 0)
	{

	}
}
