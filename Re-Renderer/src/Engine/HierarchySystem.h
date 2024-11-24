#pragma once
#include"../config.h"
#include"Components/Components.h"

namespace Re_Renderer {

    class Scene; // Forward declaration of Scene class

    class HierarchySystem {
    public:
        // Updates the global transforms of all entities in the scene
        void UpdateGlobalTransforms(Scene* scene);

    private:
        // Recursively updates global transforms of a specific entity and its children
        void updateEntityTransform(EntID entityId, Components::Transform* parentTransform, Scene* scene , bool mustUpdate);



    };
}
