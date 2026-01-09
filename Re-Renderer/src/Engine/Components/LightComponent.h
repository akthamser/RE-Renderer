#pragma once

#include<vector>
#include "../ShaderType.h"
#include<unordered_map>
#include<string>

#include<glm/glm.hpp>


namespace Re_Renderer {

	enum class LightType {

		DirectionalLight,
		PointLight,
		AmbientLight
	};

	namespace Components {





		struct Light {


			Light(LightType type = LightType::PointLight, glm::vec3 color = glm::vec3(1) , float intensity = 1 , float radius = 1)
				: Type(type),Color(color), Intensity(intensity) , Radius(radius)
			{
			}

			LightType getType() { return Type; }
			glm::vec3 getColor() { return Color; }
			float getIntensity() { return Intensity; }
			float getRadius() { return Radius; }


			void setType(LightType type) { Type = type; isDirty = true; }
			void setColor(glm::vec3 color) { Color = color; isDirty = true;}
			void setIntensity(float intensity) { Intensity = intensity; isDirty = true;}
			void setRadius(float radius) { Radius = radius;isDirty = true;}


			bool isDirty;

		private :
			LightType Type = LightType::PointLight;
			glm::vec3 Color = glm::vec3(1);
			float Intensity = 1;
			float Radius = 1;

		};

	}
}