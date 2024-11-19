#pragma once
#include<vector>
#include "../ShaderType.h"
#include<unordered_map>
#include<string>
#include"../Texture.h"
#include"Components.h"
#include<glm/glm.hpp>


namespace Re_Renderer {


	namespace Components {





		struct Material  {


			ShaderType shaderType;

			glm::vec3 Color;

			Texture* Ambient;
			Texture* Diffuse;       // Diffuse texture
			Texture* NormalMap;     // Normal map
			Texture* SpecularMap;   // Specular map



			Material(ShaderType shaderType, glm::vec3 color = glm::vec3(1))
				: shaderType(shaderType), Color(color), Ambient(nullptr), Diffuse(nullptr), NormalMap(nullptr),
				SpecularMap(nullptr)
			{}

		};

	}
}