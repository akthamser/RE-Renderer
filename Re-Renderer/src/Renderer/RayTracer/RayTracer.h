#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../Shader.h"
#include "../../ECS/Scene/Scene.h"
#include <vector>
#include"../../core/Window.h"

namespace Re_Renderer {


    // 16-byte alignment is crucial for SSBOs
    struct GPUTriangle {
        glm::vec3 v0; float pad1;
        glm::vec3 v1; float pad2;
        glm::vec3 v2; uint32_t matID;
    };

    struct GPUTMaterial {
        glm::vec3 BaseColor;
        float Roughness;
        float Metallic;
        float pad1;
        float pad2;
        float pad3;
    };

    class RayTracer {
    public:
        RayTracer(Window& window);
        ~RayTracer();

        void Render(Scene& scene);

        // 2. New Function: Updates geometry buffers
        void UpdateSceneData(Scene& scene);


    private: 
        Window& window;
        Shader m_ComputeShader;
        Shader m_ScreenQuadShader;

        GLuint m_ComputeTexture;
        GLuint m_QuadVAO, m_QuadVBO;

        // Triangle buffer 
        GLuint m_TriangleBuffer = 0;
        GLuint m_NormalBuffer = 0; 
        int m_TriangleCount = 0;

        // Materials buffer
        GLuint m_MaterialBuffer = 0;
        int m_MaterialCount = 0;


        GLuint m_IndexBuffer = 0;
        GLuint m_BVHBuffer = 0;

        void InitTexture();
        void InitQuad();
        void InitBuffers(); 


        int m_TexWidth = 0;
        int m_TexHeight = 0;

        void ResizeTexture(int width, int height);
    };
}