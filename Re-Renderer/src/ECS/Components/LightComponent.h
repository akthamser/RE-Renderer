#pragma once
#include <glm/glm.hpp>

namespace Re_Renderer {

    enum class LightType {
        Directional, 
        Point,      
        Spot         
    };

    namespace Components {

        struct Light {

            LightType type = LightType::Point;

            glm::vec3 color = glm::vec3(1.0f); 
            float intensity = 1.0f;           
            float radius = 10.0f;             

            // Spotlight specific
            float innerCutoff = 12.5f; 
            float outerCutoff = 17.5f; 

            bool isDirty = true; 

 
            Light() = default;

            Light(LightType t, glm::vec3 c, float i, float r)
                : type(t), color(c), intensity(i), radius(r) {
            }

            float getInnerCutoffCos() const { return cos(glm::radians(innerCutoff)); }
            float getOuterCutoffCos() const { return cos(glm::radians(outerCutoff)); }
        };
    }
}