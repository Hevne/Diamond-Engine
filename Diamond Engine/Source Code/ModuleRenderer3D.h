#pragma once
#include "Application.h"
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include"Primitive.h"
#include "Light.h"

#include "OpenGL.h"

class Mesh;

#define MAX_LIGHTS 1

#define SQUARE_TEXTURE_W 1024
#define SQUARE_TEXTURE_H 1024

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void OnResize(int width, int height);
	void OnGUI() override;

	void ReGenerateFrameBuffer(int w, int h);

	void CustomLoadImage(const char* fileName);

private:

	void GetCAPS(std::string& caps);
	std::string str_CAPS;

public:
	bool vsync, wireframe, cull, lightng, color_material, texture_2d;

	unsigned int framebuffer;
	unsigned int texColorBuffer;
	unsigned int rbo;

	//Mesh testMesh;
	std::vector<Mesh*> testMeshes;
	//GLubyte checkerImage[SQUARE_TEXTURE_W][SQUARE_TEXTURE_H][4];
	const char* buffer = nullptr;
	uint imgID;

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
};