#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include"shader.h"
#include"../Core/Window.h"
#include<vector>
#include<string>
#include"../../Dependencies/stb_image.h"
#include"../ECS/Scene/Scene.h"
#include"../ECS/Components/Components.h"
#include"../ECS/Scene/Entity.h"


namespace Re_Renderer {


	struct OpenGlMesh {

		unsigned int VAO, VBO, EBO;
		OpenGlMesh() = default;
	};
	
	class AssetsManager;

	class Renderer {

	public:
		Renderer(Window& window);
		~Renderer() = default;


		void setupScene(Scene& scene);
		void renderScene(Scene& scene);
		void setupTextures(AssetsManager& assetsManager);
		void resetShader();

	private:

		std::vector<Shader> m_Shaders;
		std::unordered_map<Components::Mesh*, OpenGlMesh> m_OpenGlMeshes;
		std::unordered_map<Texture*, GLuint> m_Textures;

		ShaderType activeShader = ShaderType::Count; // using Count as Null
		Shader* m_shader = nullptr;

		void setupShaders();
		Shader* getShader(ShaderType shadertype);
		void setupMesh(Components::Mesh& mesh);
		void setMaterialUniforms(const Components::Material& material,const Shader& shader, const Components::Transform& activeCameraTransform, unsigned int SkyBoxID);
		GLuint generateTexture(const Texture* texture);
		void bindTexture(Texture* texture, int textureUnit, const std::string& uniformName, const Shader& shader);
		void RenderCube();
		void uploadLightsToShader(Shader& shader, Scene& scene);

	};


}