#include"Renderer.h"



#include"../ECS/config.h"
#include"ShaderType.h"

#include"../Utils/Timer.hpp"
#include"../ECS/AssetsManager.h"

namespace Re_Renderer {

    Renderer::Renderer(Window& window) {

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
        glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(Vertex), &mesh.Vertices[0], mesh.isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

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
        m_OpenGlMeshes[&mesh].EBO = EBO;
        m_OpenGlMeshes[&mesh].VBO = VBO;

    };

    void Renderer::setupTextures(AssetsManager& assetManager) {
    
        for (auto& it : assetManager.m_TexturesCache)
        {
            auto texture = &it.second;
            m_Textures[texture] = generateTexture(texture);
        }

    };

    void Renderer::resetShader() { m_shader = nullptr; };

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

    void Renderer::setMaterialUniforms(const Components::Material& material,const Shader& shader,const Components::Transform& activeCameraTransform,unsigned int SkyBoxID) {
       
        
        shader.setVec3("material.color",material.BaseColor);
        shader.setFloat("material.Roughness",material.Roughness);
        shader.setFloat("material.Metallic",material.Metallic);
        shader.setBool("material.HasDiffuse",material.Diffuse != nullptr);

        shader.setVec3("camPos", activeCameraTransform.getPosition());
        

        bindTexture(material.Ambient, 0, "material.Ambient", shader); 
        bindTexture(material.Diffuse, 1, "material.Diffuse", shader); 
        bindTexture(material.NormalMap, 2, "material.NormalMap", shader); 
        bindTexture(material.SpecularMap, 3, "material.SpecularMap", shader);

        if (SkyBoxID != 0)
        {
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_CUBE_MAP, SkyBoxID);
            shader.setInt("skybox", 4);
        }
    };

    void Renderer::setupScene(Scene& scene) {

        auto Meshes = scene.Components.getRegistry<Components::Mesh>();
        for (auto& mesh : Meshes->getAllComponents()) {
            setupMesh(mesh);
        }

        

        if (scene.getActiveCamera() == nullptr)
            std::cout << "WARNING :: Scene Have No Camera" << std::endl;
    }


    void Renderer::RenderCube() {
        static GLuint cubeVAO = 0;
        static GLuint cubeVBO = 0;

        // Initialize (runs only once)
        if (cubeVAO == 0) {
            float vertices[] = {
                // Back face
                -1.0f, -1.0f, -1.0f,
                 1.0f,  1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                // Front face
                -1.0f, -1.0f,  1.0f,
                 1.0f, -1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,
                // Left face
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                // Right face
                 1.0f,  1.0f,  1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f,  1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f, -1.0f,  1.0f,
                 // Bottom face
                 -1.0f, -1.0f, -1.0f,
                  1.0f, -1.0f, -1.0f,
                  1.0f, -1.0f,  1.0f,
                  1.0f, -1.0f,  1.0f,
                 -1.0f, -1.0f,  1.0f,
                 -1.0f, -1.0f, -1.0f,
                 // Top face
                 -1.0f,  1.0f, -1.0f,
                  1.0f,  1.0f,  1.0f,
                  1.0f,  1.0f, -1.0f,
                  1.0f,  1.0f,  1.0f,
                 -1.0f,  1.0f, -1.0f,
                 -1.0f,  1.0f,  1.0f
            };

            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);

            glBindVertexArray(cubeVAO);

            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            glBindVertexArray(0);
        }

        // Draw
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }


    void Renderer::renderScene(Scene& scene) {

        Components::Camera* activeCamera = scene.getActiveCamera();

        if (activeCamera == nullptr)
            return;

        Components::Transform* cameraTransform = scene.Components.getRegistry<Components::Transform>()->getComponent(activeCamera->entId);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 1. Enable Depth Testing (So close things block far things)
        glEnable(GL_DEPTH_TEST);

        // 2. Enable Face Culling (So inside faces aren't drawn)
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);  // Hide the back faces
        glFrontFace(GL_CCW);  // Counter-Clockwise is "Front" (Standard)

        auto Meshes = scene.Components.getRegistry<Components::Mesh>();
        auto Materials = scene.Components.getRegistry<Components::Material>();
        auto Transforms = scene.Components.getRegistry<Components::Transform>();

        bool lightUpdated = false;
        
        for (EntID id : Meshes->getEntitiesIds())
        {
            auto mesh = Meshes->getComponent(id);
            auto material  = Materials->getComponent(id);
            auto transform = Transforms->getComponent(id);


            if (m_OpenGlMeshes.find(mesh) == m_OpenGlMeshes.end()) {
                std::cout << "Uploading new mesh to GPU: Entity " << id << std::endl;
                setupMesh(*mesh); 
            }
            else if (mesh->isDirty) {
                OpenGlMesh& glMesh = m_OpenGlMeshes[mesh];

                glBindBuffer(GL_ARRAY_BUFFER, glMesh.VBO);

                glBufferSubData(
                    GL_ARRAY_BUFFER,
                    0,
                    mesh->Vertices.size() * sizeof(Vertex),
                    mesh->Vertices.data()
                );
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                mesh->isDirty = false; 
            }

            if(material == nullptr&& (activeShader != ShaderType::Basic || m_shader == nullptr ))
            {
               // std::cout << "NO MATERIAL : " << id << std::endl;
                m_shader = getShader(ShaderType::Basic);
                activeShader = ShaderType::Basic;
                m_shader->use();
            }
            else if (material != nullptr && (activeShader != material->shaderType || m_shader == nullptr))
            {
                //Timer timer("change the Shader");
                m_shader = getShader(material->shaderType);
                activeShader = material->shaderType;
                m_shader->use();
                lightUpdated = false;
            }

            if (material != nullptr) {

                setMaterialUniforms(*material,*m_shader, *cameraTransform,scene.SkyboxTextureID);
                if (!lightUpdated)
                {
                    lightUpdated = true;
                    uploadLightsToShader(*m_shader, scene);
                }
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

        if (scene.SkyboxTextureID != 0)
        {
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_CULL_FACE);



            auto skyboxShader = getShader(ShaderType::SkyBox);
            skyboxShader->use();
            activeShader = ShaderType::SkyBox;

            // View Matrix without Translation
            glm::mat4 view = glm::mat4(glm::mat3(activeCamera->getViewMatrix()));
            skyboxShader->setMat4("view", view);
            skyboxShader->setMat4("projection", activeCamera->getProjectionMatrix());

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, scene.SkyboxTextureID);
            skyboxShader->setInt("skybox", 0);

            RenderCube();

            glEnable(GL_CULL_FACE);
            glDepthFunc(GL_LESS);
   

        }

    }
        
        
    // Renderer.cpp

    void Renderer::uploadLightsToShader(Shader& shader, Scene& scene) {
        auto lightsRegistry = scene.Components.getRegistry<Components::Light>();
        auto transformsRegistry = scene.Components.getRegistry<Components::Transform>();

        // Safety Check
        if (!lightsRegistry || !transformsRegistry) return;

        int i = 0;
        // MAX_LIGHTS must match your Shader's #define (e.g., 16)
        const int MAX_LIGHTS = 16;

        for (auto entity : lightsRegistry->getEntitiesIds()) {
            if (i >= MAX_LIGHTS) break;

            auto light = lightsRegistry->getComponent(entity);
            auto transform = transformsRegistry->getComponent(entity);

            // Skip lights that don't have a transform (safety)
            if (!transform) continue;

            std::string base = "lights[" + std::to_string(i) + "]";

            // 1. Type
            shader.setInt(base + ".type", (int)light->type);

            // 2. Geometry
            shader.setVec3(base + ".position", transform->getPosition());
            // Ensure your Transform class has getForward() implemented correctly!
            shader.setVec3(base + ".direction", transform->getForward());

            // 3. Properties
            shader.setVec3(base + ".color", light->color);
            shader.setFloat(base + ".intensity", light->intensity);
            shader.setFloat(base + ".range", light->radius);

            // 4. Spot Light Cutoffs
            // Pre-calculate cosine here to save GPU cycles
            shader.setFloat(base + ".innerCutoff", glm::cos(glm::radians(light->innerCutoff)));
            shader.setFloat(base + ".outerCutoff", glm::cos(glm::radians(light->outerCutoff)));

            i++;
        }

        // Tell the shader how many valid lights we found
        shader.setInt("lightCount", i);
    }
 
    }



