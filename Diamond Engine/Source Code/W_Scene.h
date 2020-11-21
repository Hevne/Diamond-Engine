#ifndef __W_SCENE_H__
#define __W_SCENE_H__

#include "Window.h"
#include"ImGuizmo/ImGuizmo.h"

class Application;
class W_Scene : public Window
{

public:
	W_Scene(Application* _app);
	virtual ~W_Scene();

	void InitSceneView();

	void Draw() override;

	ImGuizmo::OPERATION operation;

	Application* App;

};

#endif //__W_SCENE_H__

