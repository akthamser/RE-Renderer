#pragma once
#pragma once
#include<glm/glm.hpp>
#include<Vector>


namespace Re_Renderer {


    namespace Components {


        struct Cloth {


            struct Particle {
                glm::vec3 pos = glm::vec3(0.0f);
                glm::vec3 oldPos = glm::vec3(0.0f); 
                bool isPinned = false;
            };


            struct Spring {
                int p1; // Index of first particle
                int p2; // Index of second particle
                float restLength = 0.0f;
                float compliance = 0.0f;
            };


            std::vector<Particle> particles;
            std::vector<Spring> springs;

            glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
        };
    }
}

