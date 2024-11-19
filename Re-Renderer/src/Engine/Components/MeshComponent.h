#pragma once

#include<vector>
#include"Components.h"

namespace Re_Renderer {

		
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;

	};


	namespace Components {





		struct Mesh  {


			std::vector<Vertex> Vertices;
			std::vector<unsigned int> Indices;

			Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies) :Vertices(vertices), Indices(indicies) {};
			Mesh(const Mesh& other) :Vertices(other.Vertices), Indices(other.Indices) {};
			~Mesh() = default;



		};


	}

}