#include"Renderer.h"
#include"Scene.h"
#include"Entity.h"
#include"Components.h"
#include"../config.h"
#include"ShaderType.h"
#include"Resources.h"
#include"Timer.hpp"

namespace Re_Renderer {

    Renderer::Renderer(Window& window) :m_window(window) {

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        setupShaders();
     

    }



    void Renderer::setupShaders() {
        for (int i = 0; i < (int)ShaderType::Count; i++) {
            
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

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        m_OpenGlMeshes[&mesh].VAO = VAO;
    };

    void Renderer::setMaterialUniforms(const Components::Material& material,const Shader& shader) {
        
        shader.setVec3("color",material.Color);
    };

    void Renderer::setupScene(Scene& scene) {

        auto Meshes = scene.Components.getRegistry<Components::Mesh>();
        for (auto& mesh : Meshes->getAllComponents()) {
            setupMesh(mesh);
        }
    }

    void Renderer::renderScene(Scene& scene) {

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
      
        

            glm::mat4 view = glm::lookAt(
                glm::vec3(0.0f, 0.0f, 3.0f),  // Camera position in world space
                glm::vec3(0.0f, 0.0f, 0.0f),  // Target point in world space
                glm::vec3(0.0f, 1.0f, 0.0f)   // Up direction
            );

            glm::mat4 projection = glm::perspective(
                glm::radians(45.0f),  // Field of view in radians
                4.0f / 3.0f,          // Aspect ratio (width/height)
                0.1f,                 // Near clipping plane
                100.0f                // Far clipping plane
            );
            glm::mat4 model = transform->calculateModel();

            m_shader->setMat4("model", model);
            m_shader->setMat4("view", view);
            m_shader->setMat4("projection", projection);

            

            glBindVertexArray(m_OpenGlMeshes[mesh].VAO);

            glDrawElements(GL_TRIANGLES, mesh->Indices.size(), GL_UNSIGNED_INT, 0);
        }
        
        

 
    }



}