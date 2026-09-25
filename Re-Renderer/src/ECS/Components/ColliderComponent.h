#pragma once
#pragma once
#include<glm/glm.hpp>



namespace Re_Renderer {


	namespace Components {


        struct Collider {
            enum Type { SPHERE, PLANE, BOX } type;

            glm::vec3 offset = glm::vec3(0.0f);
            float radius = 1.0f;       // Sphere
            float planeHeight = 0.0f;  // Plane
            glm::vec3 boxSize = glm::vec3(1.0f); // Box

            bool drawGizmos = false;
        };
	}
}

