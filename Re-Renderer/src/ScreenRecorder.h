#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include<vector>



class ScreenRecorder {

public:
	ScreenRecorder(int width,int height,int frames,const std::string& fileName,bool overRide = false);
	~ScreenRecorder();

	void captureFrame();
	void StartRecording();
private:
	int m_height, m_width;
	FILE* ffmpegPipe;
	std::string commande;

};