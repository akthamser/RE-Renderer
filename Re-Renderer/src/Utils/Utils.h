#pragma once


#include <iostream>
#include <fstream>
#include <sstream>
#include "../../Dependencies/stb_image.h"
#include <vector>
#include <string>

#include <glad/glad.h> 



std::string ReadShaderfromFile(std::string shaderPath);
GLuint LoadCubemap(std::vector<std::string> faces);