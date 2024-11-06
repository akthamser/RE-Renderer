#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include"shader.h"
#include"Window.h"
#include<vector>
#include<string>
#include"../Dependencies/stb_image.h"
#include"Scene.h"
#include"Components.h"

namespace Re_Renderer {



	class CameraSystem {

	public:
		CameraSystem(Window& window);
		~CameraSystem() = default;


		void UpdateCameras(Scene& scene);



	private:
		Window& m_window;
	

	};


}