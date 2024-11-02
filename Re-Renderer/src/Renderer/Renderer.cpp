#include"Renderer.h"
#include"Scene.h"
#include"Entity.h"
#include"Components.h"
#include"../config.h"
#include"ShaderType.h"


namespace Re_Renderer {

    Renderer::Renderer(Window& window) :m_window(window) {

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }


        m_shader = new Shader("vertexShader.vert", "fragmentShader.frag");







        std::vector<Vertex> cubeVertices = {
            // Positions           // Normals           // Texture Coords
            { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f) }, // Back bottom left
            { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f) }, // Back bottom right
            { glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f) }, // Back top right
            { glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f) }, // Back top left
            { glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) }, // Front bottom left
            { glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) }, // Front bottom right
            { glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) }, // Front top right
            { glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) }  // Front top left
        };

        std::vector<unsigned int> cubeIndices = {
            // Back face
            0, 1, 2,
            0, 2, 3,
            // Front face
            4, 5, 6,
            4, 6, 7,
            // Left face
            0, 3, 7,
            0, 7, 4,
            // Right face
            1, 5, 6,
            1, 6, 2,
            // Bottom face
            0, 1, 5,
            0, 5, 4,
            // Top face
            3, 2, 6,
            3, 6, 7
        };
      
        Entity* cube = m_scene.getEntityByID(m_scene.CreateEntity("cube"));
        auto transform = cube->addComponent<Components::Transform>();

    

        auto mesh = cube->addComponent<Components::Mesh>(cubeVertices, cubeIndices);

        auto material = cube->addComponent<Components::Material>(ShaderType::Basic);
        material->colorProperties["color"] = glm::vec3(0,1,0);
 

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, mesh->Vertices.size() * sizeof(Vertex), &mesh->Vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->Indices.size() * sizeof(unsigned int), &mesh->Indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex) , (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        m_OpenGlMeshes[mesh].VAO =  VAO;

    }

    Renderer::~Renderer() {
        delete m_shader;
    }

    void Renderer::Render() {

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        auto Meshes = m_scene.Components.getRegistry<Components::Mesh>();
        auto Materials = m_scene.Components.getRegistry<Components::Material>();
        auto Transforms = m_scene.Components.getRegistry<Components::Transform>();

        for (EntID id : Meshes->getEntitiesIds())
        {
             m_shader->use();
        
            Components::Mesh* mesh = Meshes->getComponent(id);
            auto material = Materials->getComponent(id);
            if (material != nullptr) {

                glm::vec3 col = material->colorProperties["color"];
                m_shader->setVec3("color", col);
            }
            auto transform = Transforms->getComponent(id);
      
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
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        

 
    }



}