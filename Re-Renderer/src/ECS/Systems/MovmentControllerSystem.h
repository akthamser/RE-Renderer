#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"../../Core/Window.h"
#include"../Components/Components.h"

namespace Re_Renderer{


class Scene;
class MovmentControllerSystem {


public:
	MovmentControllerSystem(Window& window) : m_window(window) { setup(); };
	~MovmentControllerSystem() = default;


	void UpdateMovment(Scene& scene,float deltatime);

	

private :
	Window& m_window;
	double lastXpos, lastYpos;

	void setup();
	void move(Components::Transform* transform, Components::MovmentController* mc, glm::vec2& Input,float deltatime);
	void look(Components::Transform* transform, Components::MovmentController* mc, glm::vec2& deltamouse, float deltatime);



};





}