#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ImGui/imconfig.h"
#include "ImGui/imgui.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled), mouseSensitivity(0.50f), cameraSpeed(1.f), cameraMovement(0.f, 0.f, 0.f)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(5.0f, 3.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	LookAt(vec3(0.f, 0.f, 0.f));

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

void ModuleCamera3D::OnGUI()
{
	if (ImGui::CollapsingHeader("3D Input Settings"))
	{
		ImGui::TextWrapped("Camera Rotation Speed"); ImGui::SameLine(); ImGui::PushItemWidth(100.f); ImGui::DragFloat("##crs", &mouseSensitivity, 0.01f, 0.f, 999.f);
		ImGui::TextWrapped("Camera Movement Speed"); ImGui::SameLine(); ImGui::PushItemWidth(100.f); ImGui::DragFloat("##cms", &cameraSpeed, 0.01f, 0.f, 999.f);
	}
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{

	
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

void ModuleCamera3D::ProcessSceneKeyboard()
{
	cameraMovement.Set(0.f, 0.f, 0.f);
	const float dt = App->GetDT();

	float speed = cameraSpeed * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed *= 2.f;

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) cameraMovement.y += speed;
	//if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) cameraMovement -= Z * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) cameraMovement += Z * speed;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) cameraMovement -= X * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) cameraMovement += X * speed;

	if (App->input->GetMouseZ() != 0)
	{
		cameraMovement += Z * speed * -App->input->GetMouseZ() * 250;
	}

	// Mouse motion ----------------

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN /*|| App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN*/)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_WarpMouseInWindow(App->window->window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_UP /*|| App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP*/)
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_WarpMouseInWindow(App->window->window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	}

	//ASK: Is this really the best way to rotate the camera? Maybe i should use a matrix
	//TODO: Camera rotation should not be affected by the program framerate
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		FreeRotation(dt);
	}

	//Rotate around 0,0,0
	//ASK: Should i also include Right alt?
	//Maybe we could use quaternions?
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		OrbitalRotation(vec3(0, 0, 0), dt);


	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		FocusCamera(vec3(0.f, 0.f, 0.f), 10.f);

	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
		PanCamera(dt);

	Move(cameraMovement);

}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

//Could be a good idea to use quaternions? Would it be faster?
//BUG: Camera will bug when the the horizontal rotation is almost 
//equal as the target X and Z coords
void ModuleCamera3D::OrbitalRotation(vec3 center, float dt)
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	if (dx != 0)
	{
		float DeltaX = (float)dx * mouseSensitivity;

		//Get vector diference
		float distance = length(Position - center);
		vec3 ref = normalize(Position - center);

		//Rotate diference
		ref = rotate(ref, DeltaX, vec3(0.0f, 1.0f, 0.0f));

		//Move camera position to new diference
		Position = center + ref * distance;

	}
	if (dy != 0)
	{

		//BUG: Weird bug when looking at the same Y
		float DeltaY = (float)dy * mouseSensitivity;

		float distance = length(Position - center);
		vec3 ref = normalize(Position - center);
		ref = rotate(ref, DeltaY, -cross(ref, vec3(0.0f, 1.0f, 0.0f)));

		//debug = center + ref * distance;

		Position = center + ref * distance;
	}


	//Reference = center;
	LookAt(center);
}

void ModuleCamera3D::FreeRotation(float dt)
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	//Position -= Reference;

	/*vec3 positionSave = Position;
	Position = vec3(0.f, 0.f, 0.f);*/

	if (dx != 0)
	{
		float DeltaX = (float)dx * mouseSensitivity;

		vec3 rotationvector = vec3(0.0f, 1.0f, 0.0f);
		X = rotate(X, DeltaX, rotationvector);
		Y = rotate(Y, DeltaX, rotationvector);
		Z = rotate(Z, DeltaX, rotationvector);

	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * mouseSensitivity;

		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);
		}
	}

	/*Position = positionSave;*/
	//Position = Reference + Z * length(Position);
}

void ModuleCamera3D::FocusCamera(vec3 center, float offset)
{
	Position = center;

	Position += normalize(Z) * offset;
}

void ModuleCamera3D::PanCamera(float dt)
{

	//WARNING: Need to normalize movement
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	if (dx != 0 || dy != 0) 
	{
		//vec3 mouseDir = normalize(vec3(dx, dy, 0.f));

		//Move(X * (mouseDir.x * cameraSpeed * dt));
		//Move(-Y * (mouseDir.y * cameraSpeed * dt));

		vec3 movVector((X * dx) + (-Y * dy));

		Position += movVector * dt;

	}
}
