#pragma once
#include<vector>
#include<string>
#include<glm/glm.hpp>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"shader.h"


struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
};


class Mesh {

public:
	std::vector<Vertex> Verticies;
	std::vector<unsigned int> Indicies;
	std::vector<Texture> Textures;

	Mesh(std::vector<Vertex> verticies, std::vector<unsigned int> indicies, std::vector<Texture> textures);
	
	
	

private:
	unsigned int VAO, VBO, EBO;

	void setup();
	
};