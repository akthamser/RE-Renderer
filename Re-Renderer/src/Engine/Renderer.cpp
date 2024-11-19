#include"Renderer.h"
#include"Scene/Scene.h"
#include"Scene/Entity.h"
#include"components/Components.h"
#include"../config.h"
#include"ShaderType.h"
#include"Resources.h"
#include"Timer.hpp"
#include"AssetsManager.h"

namespace Re_Renderer {

    Renderer::Renderer(Window& window) :m_window(window) {

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        setupShaders();
        glEnable(GL_DEPTH_TEST);

    }



    void Renderer::setupShaders() {
        m_Shaders.reserve((size_t)ShaderType::Count); 
        for (int i = 0; i < (size_t)ShaderType::Count; i++) {
            
            m_Shaders.emplace_back(shaderPaths[i].vertexPath.c_str(), shaderPaths[i].fragmentPath.c_str());
        
        }
    };
    Shader* Renderer::getShader(ShaderType shadertype) {
        return &m_Shaders[(size_t)shadertype];

    };

    void Renderer::setupMesh(Components::Mesh& mesh) {
        
        GLuint VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(Vertex), &mesh.Vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(unsigned int), &mesh.Indices[0], GL_STATIC_DRAW);




        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        glEnableVertexAttribArray(4);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        glBindVertexArray(0);

        m_OpenGlMeshes[&mesh].VAO = VAO;
    };

    void Renderer::setupTextures(AssetsManager& assetManager) {
    
        for (auto& it : assetManager.m_TexturesCache)
        {
            auto texture = &it.second;
            m_Textures[texture] = generateTexture(texture);
        }

    };


    GLuint Renderer::generateTexture(const Texture* texture) {

        if (!texture || !texture->data) {
            return 0; // Return 0 if texture is null or data is not loaded
        }

        GLuint textureID; 
        glGenTextures(1, &textureID); 
        glBindTexture(GL_TEXTURE_2D, textureID); 

        GLenum format = GL_RGB;
        if (texture->NrChannels == 1)
            format = GL_RED;
        else if (texture->NrChannels == 3)
            format = GL_RGB;
        else if (texture->NrChannels == 4)
            format = GL_RGBA;

        // Upload the texture data to the GPU
        glTexImage2D(GL_TEXTURE_2D, 0, format, texture->Width, texture->Height, 0,format , GL_UNSIGNED_BYTE, texture->data);
        glGenerateMipmap(GL_TEXTURE_2D); 
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture 
        return textureID; 
    };


    void Renderer::bindTexture(Texture* texture, int textureUnit, const std::string& uniformName, const Shader& shader) {
        if (!texture) return; // Skip if texture is null

        GLuint textureID;
        if (m_Textures.find(texture) == m_Textures.end()) {
            textureID = generateTexture(texture);
            m_Textures[texture] = textureID;
        }
        else {
            textureID = m_Textures[texture];
        }

        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureID);
        shader.setInt(uniformName, textureUnit);
    }

    void Renderer::setMaterialUniforms(const Components::Material& material,const Shader& shader) {
       
        
        shader.setVec3("color",material.Color);

        bindTexture(material.Ambient, 0, "material.Ambient", shader); 
        bindTexture(material.Diffuse, 1, "material.Diffuse", shader); 
        bindTexture(material.NormalMap, 2, "material.NormalMap", shader); 
        bindTexture(material.SpecularMap, 3, "material.SpecularMap", shader);

    };

    void Renderer::setupScene(Scene& scene) {

        auto Meshes = scene.Components.getRegistry<Components::Mesh>();
        for (auto& mesh : Meshes->getAllComponents()) {
            setupMesh(mesh);
        }

        

        if (scene.getActiveCamera() == nullptr)
            std::cout << "WARNING :: Scene Have No Camera" << std::endl;
    }

    void Renderer::renderScene(Scene& scene) {

        Components::Camera* activeCamera = scene.getActiveCamera();

        if (activeCamera == nullptr)
            return;

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        auto Meshes = scene.Components.getRegistry<Components::Mesh>();
        auto Materials = scene.Components.getRegistry<Components::Material>();
        auto Transforms = scene.Components.getRegistry<Components::Transform>();


        
        for (EntID id : Meshes->getEntitiesIds())
        {
            auto mesh = Meshes->getComponent(id);
            auto material  = Materials->getComponent(id);
            auto transform = Transforms->getComponent(id);



            if (activeShader != material->shaderType || m_shader == nullptr)
            {
                Timer timer("change the Shader");
                m_shader = getShader(material->shaderType);
                activeShader = material->shaderType;
                m_shader->use();

            }

            if (material != nullptr) {

                setMaterialUniforms(*material,*m_shader);
            }
      
        

          
            glm::mat4 model = transform->getModel();
            glm::mat4 view = activeCamera->getViewMatrix();
            glm::mat4 projection = activeCamera->getProjectionMatrix();


            m_shader->setMat4("model", model);
            m_shader->setMat4("view", view);
            m_shader->setMat4("projection", projection);

            

            glBindVertexArray(m_OpenGlMeshes[mesh].VAO);

            glDrawElements(GL_TRIANGLES, mesh->Indices.size(), GL_UNSIGNED_INT, 0);
        }
        
        

 
    }



}