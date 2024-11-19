#pragma once
#include<glm/glm.hpp>
#include<vector>
#include"Components/Components.h"

namespace Re_Renderer {



    const Components::Mesh cubeMesh(
        //vertices
        {
            // Positions           // Normals           // Texture Coords
            { glm::vec3(-0.5f, -0.5f, -0.5f),glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f),glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 0.0f, -1.0f)  },// Back bottom left
            { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f),glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 0.0f, -1.0f)  },// Back bottom right
            { glm::vec3(0.5f, 0.5f, -0.5f),  glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f),glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 0.0f, -1.0f)  },// Back top right
            { glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f),glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 0.0f, -1.0f)  },// Back top left
            { glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) , glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 0.0f, -1.0f)} ,// Front bottom left
            { glm::vec3(0.5f, -0.5f, 0.5f),  glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) , glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 0.0f, -1.0f)} ,// Front bottom right
            { glm::vec3(0.5f, 0.5f, 0.5f),   glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) , glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 0.0f, -1.0f)} ,// Front top right
            { glm::vec3(-0.5f, 0.5f, 0.5f),  glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) ,  glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 0.0f, -1.0f)} // Front top left
        },
        //indices
        {
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
        }
        );
}