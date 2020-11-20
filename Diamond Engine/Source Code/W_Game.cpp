#include "W_Game.h"
#include "C_Camera.h"
#include"Globals.h"

W_Game::W_Game() : Window(), targetCamera(nullptr)
{
	name = "Game";
}

W_Game::~W_Game()
{
}

void W_Game::Draw()
{
	//TODO: Could we call this from the base Window class? Like with components? Maybe to avoid duplicated code?
	if (ImGui::Begin(name.c_str(), NULL/*, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize*/))
	{
		//Can't use if(true) return; because we need to call ImGui::End();
		if (targetCamera != nullptr && targetCamera->framebuffer != -1) {
			//LOG(LogType::L_WARNING, "Frame buffer game id: %d", targetCamera->framebuffer);
			targetCamera->SetAspectRatio(ImGui::GetContentRegionAvail().x / ImGui::GetContentRegionAvail().y);
			ImGui::Image((ImTextureID)targetCamera->texColorBuffer, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
		}

	}
	ImGui::End();
}

void W_Game::SetTargetCamera(C_Camera* _cam)
{
	targetCamera = _cam;
	targetCamera->ReGenerateBuffer(1920, 1080);
}