#include "RayTracer.h"
#include <iostream>
#include <vector>

#include"BVH.h"

namespace Re_Renderer {

    RayTracer::RayTracer(Window& window)
        : window(window),
        m_ComputeShader("raytracer.comp"),
        m_ScreenQuadShader("screenQuad.vert", "screenQuad.frag")
    {
        m_TexWidth = window.Width;
        m_TexHeight = window.Height;

        InitTexture();
        InitQuad();
        InitBuffers();
    }

    RayTracer::~RayTracer() {}

    void RayTracer::InitBuffers() {

        // --- EXISTING BUFFERS ---
        // Binding 2: Triangles
        glGenBuffers(1, &m_TriangleBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TriangleBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_TriangleBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Binding 3: Materials
        glGenBuffers(1, &m_MaterialBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_MaterialBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_MaterialBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Binding 4: Normals
        glGenBuffers(1, &m_NormalBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_NormalBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_NormalBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // --- NEW BVH BUFFERS ---
        // Binding 5: BVH Nodes
        glGenBuffers(1, &m_BVHBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVHBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_BVHBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Binding 6: Sorted Indices
        glGenBuffers(1, &m_IndexBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_IndexBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_IndexBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void RayTracer::UpdateSceneData(Scene& scene) {
        std::vector<GPUTriangle> triangles;
        std::vector<GPUTMaterial> materials;
        std::vector<glm::vec4> normals;

        triangles.clear();
        materials.clear();
        normals.clear();

        auto meshRegistry = scene.Components.getRegistry<Components::Mesh>();
        auto transformRegistry = scene.Components.getRegistry<Components::Transform>();
        auto materialRegistry = scene.Components.getRegistry<Components::Material>();

        // 1. GATHER GEOMETRY
        for (EntID entityID : meshRegistry->getEntitiesIds()) {
            auto* mesh = meshRegistry->getComponent(entityID);
            auto* transform = transformRegistry->getComponent(entityID);

            if (!transform) continue;

            // --- Material Handling ---
            GPUTMaterial defaultMat;
            defaultMat.BaseColor = glm::vec3(1.0f, 0.0f, 1.0f);
            defaultMat.Roughness = 0.5f;
            defaultMat.Metallic = 0.0f;
            materials.push_back(defaultMat);

            auto mat = materialRegistry->getComponent(entityID);
            uint32_t matid = 0;
            if (mat != nullptr) {
                GPUTMaterial gpu_mat;
                gpu_mat.BaseColor = mat->BaseColor;
                gpu_mat.Roughness = mat->Roughness;
                gpu_mat.Metallic = mat->Metallic;
                materials.push_back(gpu_mat);
                matid = materials.size() - 1;
            }

            // --- Transform Handling ---
            glm::mat4 modelMatrix = transform->getModel();
            glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

            // --- Triangle Processing ---
            for (size_t i = 0; i < mesh->Indices.size(); i += 3) {
                GPUTriangle t;

                // Positions
                glm::vec3 p0 = mesh->Vertices[mesh->Indices[i]].Position;
                glm::vec3 p1 = mesh->Vertices[mesh->Indices[i + 1]].Position;
                glm::vec3 p2 = mesh->Vertices[mesh->Indices[i + 2]].Position;

                t.v0 = glm::vec3(modelMatrix * glm::vec4(p0, 1.0f));
                t.v1 = glm::vec3(modelMatrix * glm::vec4(p1, 1.0f));
                t.v2 = glm::vec3(modelMatrix * glm::vec4(p2, 1.0f));
                t.matID = matid;
                triangles.push_back(t);

                // Normals
                glm::vec3 n0 = mesh->Vertices[mesh->Indices[i]].Normal;
                glm::vec3 n1 = mesh->Vertices[mesh->Indices[i + 1]].Normal;
                glm::vec3 n2 = mesh->Vertices[mesh->Indices[i + 2]].Normal;

                glm::vec3 wn0 = normalize(normalMatrix * n0);
                glm::vec3 wn1 = normalize(normalMatrix * n1);
                glm::vec3 wn2 = normalize(normalMatrix * n2);

                normals.push_back(glm::vec4(wn0, 0.0f));
                normals.push_back(glm::vec4(wn1, 0.0f));
                normals.push_back(glm::vec4(wn2, 0.0f));
            }
        }

        m_TriangleCount = (int)triangles.size();
        m_MaterialCount = (int)materials.size();

        // 2. UPLOAD STANDARD BUFFERS
        if (m_TriangleCount > 0) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TriangleBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(GPUTriangle), triangles.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        if (m_MaterialCount > 0) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_MaterialBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, materials.size() * sizeof(GPUTMaterial), materials.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        if (!normals.empty()) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_NormalBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, normals.size() * sizeof(glm::vec4), normals.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        // 3. BUILD & UPLOAD BVH
        if (m_TriangleCount > 0) {
            static BVHBuilder bvhBuilder; // Static prevents re-allocation every frame
            bvhBuilder.Build(triangles);

            // Upload Nodes (Binding 5)
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVHBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, bvhBuilder.nodes.size() * sizeof(GPUBVHNode), bvhBuilder.nodes.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            // Upload Sorted Indices (Binding 6)
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_IndexBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, bvhBuilder.finalTriIndices.size() * sizeof(int), bvhBuilder.finalTriIndices.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        // 4. UPLOAD LIGHTS (Uniforms)
        m_ComputeShader.use();
        m_ComputeShader.setInt("u_TriangleCount", m_TriangleCount);
        m_ComputeShader.setInt("u_MaterialCount", m_MaterialCount);

        auto lightsRegistery = scene.Components.getRegistry<Components::Light>();
        int lightIdx = 0;
        const int MAX_LIGHTS = 16;

        for (auto entityID : lightsRegistery->getEntitiesIds()) {
            if (lightIdx >= MAX_LIGHTS) break;
            auto* light = lightsRegistery->getComponent(entityID);
            auto* transform = transformRegistry->getComponent(entityID);
            if (!transform) continue;

            std::string base = "lights[" + std::to_string(lightIdx) + "]";
            m_ComputeShader.setInt(base + ".type", (int)light->type);
            m_ComputeShader.setVec3(base + ".color", light->color);
            m_ComputeShader.setFloat(base + ".intensity", light->intensity);
            m_ComputeShader.setVec3(base + ".position", transform->getPosition());
            m_ComputeShader.setVec3(base + ".direction", transform->getForward());
            m_ComputeShader.setFloat(base + ".range", light->radius);
            m_ComputeShader.setFloat(base + ".innerCutoff", glm::cos(glm::radians(light->innerCutoff)));
            m_ComputeShader.setFloat(base + ".outerCutoff", glm::cos(glm::radians(light->outerCutoff)));

            lightIdx++;
        }
        m_ComputeShader.setInt("lightCount", lightIdx);
    }

    // --- 1. Texture Setup (The "Canvas") ---
    // This creates a 2D image on the GPU that the Compute Shader can write to.
    void RayTracer::InitTexture() {
        glGenTextures(1, &m_ComputeTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_ComputeTexture);

        // Settings: Clamp to edge so we don't get weird borders
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Filtering: Linear makes it smooth if we zoom in
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // GL_RGBA32F: We need high precision (Floats) for physics calculations
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window.Width , window.Height, 0, GL_RGBA, GL_FLOAT, NULL);

        // Bind to "Image Unit 0" so the shader can access it via 'binding = 0'
        // GL_READ_WRITE: Important! Allows mixing old frames with new ones (Accumulation)
        glBindImageTexture(0, m_ComputeTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    }

    // --- 2. Quad Setup (The "Screen") ---
    // Standard OpenGL stuff to draw a rectangle covering the whole screen.
    void RayTracer::InitQuad() {
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        };
        glGenVertexArrays(1, &m_QuadVAO);
        glGenBuffers(1, &m_QuadVBO);
        glBindVertexArray(m_QuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    // --- 3. The Render Loop ---
    void RayTracer::Render(Scene& scene) {


        if (window.Width != m_TexWidth || window.Height != m_TexHeight) {
            ResizeTexture(window.Width, window.Height);
        }


        m_ComputeShader.use();

        // Send Camera Data so we can look around
        Components::Camera* camera = scene.getActiveCamera();
        if (camera) {
            Components::Transform* transform = scene.Components.getRegistry<Components::Transform>()->getComponent(camera->entId);
            if (transform) {
                glm::vec3 camDir = camera->getForward(transform->getRotation());
                glm::vec3 camUp = camera->getUp(transform->getRotation());
                glm::vec3 camRight = glm::cross(camDir, camUp);
                float fovScale = tan(glm::radians(camera->getFOV()) * 0.5f);

                m_ComputeShader.setVec3("u_CamPos", transform->getPosition());
                m_ComputeShader.setVec3("u_CamDir", camDir);
                m_ComputeShader.setVec3("u_CamUp", camUp);
                m_ComputeShader.setVec3("u_CamRight", camRight);
                m_ComputeShader.setFloat("u_Fov_Scale", fovScale);
            }
        }

        if (scene.SkyboxTextureID != 0) {
            glActiveTexture(GL_TEXTURE1); // Use Unit 1
            glBindTexture(GL_TEXTURE_CUBE_MAP, scene.SkyboxTextureID);
            m_ComputeShader.setInt("u_Skybox", 1); // Tell shader to read Unit 1
        }

        // Run the Compute Shader
        // We divide by 8 because the shader works in groups of 8x8 pixels
        glDispatchCompute((unsigned int)ceil(window.Width / 8.0), (unsigned int)ceil(window.Height / 8.0), 1);

        // Wait for it to finish before drawing
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glDisable(GL_DEPTH_TEST);
        // Draw the result to the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_ScreenQuadShader.use();
        m_ScreenQuadShader.setInt("screenTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_ComputeTexture);

        glBindVertexArray(m_QuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);
    }

    void RayTracer::ResizeTexture(int width, int height) {
        if (width <= 0 || height <= 0) return;

        // 1. Re-allocate memory for the existing texture ID
        glBindTexture(GL_TEXTURE_2D, m_ComputeTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

        // 2. Update Image Binding
        glBindImageTexture(0, m_ComputeTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        glBindTexture(GL_TEXTURE_2D, 0);

        // 3. Update internal trackers
        m_TexWidth = width;
        m_TexHeight = height;

        std::cout << "RayTracer Resized to: " << width << "x" << height << std::endl;
    }
}