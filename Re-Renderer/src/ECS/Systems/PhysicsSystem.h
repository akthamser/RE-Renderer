#pragma once
#include "../Scene/Scene.h" 
#include "../Components/Components.h"
#include <algorithm> 

namespace Re_Renderer {

    class PhysicsSystem {
    public:
        static void Update(Scene& scene, float dt) {

      
            auto clothRegistry = scene.Components.getRegistry<Components::Cloth>();
            auto colliderRegistry = scene.Components.getRegistry<Components::Collider>();
            auto transformRegistry = scene.Components.getRegistry<Components::Transform>();
            auto meshRegistry = scene.Components.getRegistry<Components::Mesh>();

            
            glm::vec3 gravity(0.0f, -9.81f, 0.0f);

   
            for (auto entityID : clothRegistry->getEntitiesIds()) {
                auto* cloth = clothRegistry->getComponent(entityID);
                auto* mesh = meshRegistry->getComponent(entityID);
                auto* clothTransform = transformRegistry->getComponent(entityID);

       
                for (auto& p : cloth->particles) {
                    if (p.isPinned) continue; // Don't move pinned points

               
                    glm::vec3 velocity = p.pos - p.oldPos;
                    p.oldPos = p.pos;

                    p.pos += velocity * 0.99f + gravity * (dt * dt);
                }

                for (int i = 0; i < 15; i++) {

                    for (auto& s : cloth->springs) {
                        auto& p1 = cloth->particles[s.p1];
                        auto& p2 = cloth->particles[s.p2];

                        glm::vec3 delta = p2.pos - p1.pos;
                        float currentDist = glm::length(delta);

                        if (currentDist < 0.0001f) {
                            delta = glm::vec3(0.001f, 0.0f, 0.0f);
                            currentDist = 0.001f;
                        }

                        float w1 = p1.isPinned ? 0.0f : 1.0f;
                        float w2 = p2.isPinned ? 0.0f : 1.0f;
                        float wSum = w1 + w2;

                        if (wSum == 0.0f) continue;

                        float alpha = s.compliance / (dt * dt);

                        float C = currentDist - s.restLength; 

                        float multiplier = C / (wSum + alpha);

                        glm::vec3 direction = delta / currentDist; 
                        glm::vec3 correction = direction * multiplier;

                        p1.pos += correction * w1;
                        p2.pos -= correction * w2;
                    }

                    glm::vec3 clothPos = glm::vec3(0.0f);
                    if (clothTransform) {
                        clothPos = clothTransform->getPosition();
                    }

                    for (auto colID : colliderRegistry->getEntitiesIds()) {
                        auto* col = colliderRegistry->getComponent(colID);
                        auto* t = transformRegistry->getComponent(colID);

                        for (auto& p : cloth->particles) {
                            if (p.isPinned) continue;


                            glm::vec3 worldPos = p.pos + clothPos;
                            glm::vec3 oldWorldPos = p.oldPos + clothPos;

                            switch (col->type) {
                                
                            case Components::Collider::SPHERE: {
                                glm::vec3 center = t->getPosition() + col->offset;
                                float r = col->radius + 0.05f;
                                glm::vec3 dir = worldPos - center;
                                if (glm::length(dir) < r) {
                                    worldPos = center + glm::normalize(dir) * r;
                                    glm::vec3 vel = worldPos - oldWorldPos;
                                    oldWorldPos = worldPos - (vel * 0.5f);
                                }
                                break;
                            }
                                                            
                            case Components::Collider::PLANE: {
                                if (worldPos.y < col->planeHeight) {
                                    worldPos.y = col->planeHeight + 0.01f;
                                    glm::vec3 vel = worldPos - oldWorldPos;
                                    oldWorldPos = worldPos - (vel * 0.5f);
                                }
                                break;
                            }
  
                            case Components::Collider::BOX: {
                                glm::vec3 center = t->getPosition() + col->offset;
                                glm::vec3 half = (col->boxSize * t->getLocalScale()) * 0.5f;

                                float radius = 0.05f;
                                glm::vec3 minB = center - half - glm::vec3(radius);
                                glm::vec3 maxB = center + half + glm::vec3(radius);

             
                                if (worldPos.x > minB.x && worldPos.x < maxB.x &&
                                    worldPos.y > minB.y && worldPos.y < maxB.y &&
                                    worldPos.z > minB.z && worldPos.z < maxB.z)
                                {
              
                                    worldPos.y = maxB.y;

                                    glm::vec3 vel = worldPos - oldWorldPos;
                                    oldWorldPos = worldPos - (vel * 0.8f);
                                }
                                break;
                            }
                            } 


                            p.pos = worldPos - clothPos;
                            p.oldPos = oldWorldPos - clothPos;

                        } 
                    } 
                }

                // update mesh
                for (size_t k = 0; k < cloth->particles.size(); k++) {
                    if (k < mesh->Vertices.size()) {
                        mesh->Vertices[k].Position = cloth->particles[k].pos;
                    }
                }

                // Recalculate Normals
                for (size_t k = 0; k < mesh->Indices.size(); k += 3) {
                    uint32_t i0 = mesh->Indices[k];
                    uint32_t i1 = mesh->Indices[k + 1];
                    uint32_t i2 = mesh->Indices[k + 2];

                    glm::vec3 v0 = mesh->Vertices[i0].Position;
                    glm::vec3 v1 = mesh->Vertices[i1].Position;
                    glm::vec3 v2 = mesh->Vertices[i2].Position;

                    glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

                    mesh->Vertices[i0].Normal = normal; 
                    mesh->Vertices[i1].Normal = normal;
                    mesh->Vertices[i2].Normal = normal;
                }
                mesh->isDirty = true;
            }
        }
    };
}