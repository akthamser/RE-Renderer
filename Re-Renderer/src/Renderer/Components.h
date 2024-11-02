#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/quaternion.hpp>
#include<vector>
#include <glm/gtc/constants.hpp> 
#include "ShaderType.h"
#include<unordered_map>
#include<string>

namespace Re_Renderer {



	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};


	namespace Components {
	




		struct Component {

		};
	
		struct Transform : public Component {

			glm::vec3 Position = glm::vec3(0);
			glm::vec3 Rotation = glm::vec3(0);
			glm::vec3 Scale = glm::vec3(1);

			Transform() = default;
			Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : Position(position), Rotation(rotation), Scale(scale) {};

			// Model Mat4
			glm::mat4 calculateModel() {

				glm::mat4 rot = glm::toMat4(glm::quat(Rotation));

				return glm::translate(glm::mat4(1),Position)
						* rot
						* glm::scale(glm::mat4(1),Scale);
			}


		};
	


		struct Mesh : public Component {


			std::vector<Vertex> Vertices;
			std::vector<unsigned int> Indices;

			Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies):Vertices(vertices),Indices(indicies) {};


		


		};

		struct Material : public Component {


			ShaderType shaderType;

			std::unordered_map<std::string, std::string> textures; 
			std::unordered_map<std::string, float> floatProperties; 
			std::unordered_map<std::string, glm::vec3> colorProperties;

			Material(ShaderType shaderType,
				const std::unordered_map<std::string, std::string>& textures = {},
				const std::unordered_map<std::string, float>& floatProperties = {},
				const std::unordered_map<std::string, glm::vec3>& colorProperties = {})
				: shaderType(shaderType), 
				textures(textures),     
				floatProperties(floatProperties), 
				colorProperties(colorProperties) 
			{
			}

		};


	}



}

