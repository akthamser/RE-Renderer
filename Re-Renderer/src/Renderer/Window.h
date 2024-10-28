#pragma once
#include<GLFW/glfw3.h>
#include<iostream>

class Window {

public :
	const char* Title;
	int Width, Height;

	Window(int width,int height,const char* title,bool fullScreen = false);
	~Window();
	bool ShouldClose();
	void SwapBuffers();
	void MakeContextCurrent();

private:
	GLFWwindow* m_window;
	static void window_size_call_back(GLFWwindow* window, int width, int height);

};