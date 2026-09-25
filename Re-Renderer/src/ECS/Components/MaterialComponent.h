#pragma once
#include<vector>
#include "../../Renderer/ShaderType.h"
#include<unordered_map>
#include<string>
#include"../../Renderer/Texture.h"
#include<glm/glm.hpp>


namespace Re_Renderer {


	namespace Components {





		struct Material  {


			ShaderType shaderType;

			glm::vec3 BaseColor;
			float Roughness;       
			float Metallic;

			Texture* Ambient;
			Texture* Diffuse;       // Diffuse texture
			Texture* NormalMap;     // Normal map
			Texture* SpecularMap;   // Specular map

			Material() = default;

			Material(ShaderType shaderType, glm::vec3 color = glm::vec3(1))
				: shaderType(shaderType), BaseColor(color), Ambient(nullptr), Diffuse(nullptr), NormalMap(nullptr),
				SpecularMap(nullptr)
			{}

		};

	}
}