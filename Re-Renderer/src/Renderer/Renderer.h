#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include"shader.h"
#include"Window.h"
#include<vector>
#include<string>
#include"../Dependencies/stb_image.h"
#include"Scene.h"
#include"Components.h"

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


	private:

		Window& m_window;
		std::vector<Shader> m_Shaders;
		std::unordered_map<Components::Mesh*, OpenGlMesh> m_OpenGlMeshes;
		std::unordered_map<Texture*, GLuint> m_Textures;

		ShaderType activeShader = ShaderType::Count; // using Count as Null
		Shader* m_shader = nullptr;

		void setupShaders();
		Shader* getShader(ShaderType shadertype);
		void setupMesh(Components::Mesh& mesh);
		void setMaterialUniforms(const Components::Material& material,const Shader& shader);
		GLuint generateTexture(const Texture* texture);
		void bindTexture(Texture* texture, int textureUnit, const std::string& uniformName, const Shader& shader);

	};


}