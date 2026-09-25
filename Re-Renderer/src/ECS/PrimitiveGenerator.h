#pragma once

#include "Components/Components.h"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Re_Renderer {

    class PrimitiveGenerator {
    public:

        static void CreateCube(Components::Mesh& mesh) {
            mesh.Vertices.clear();
            mesh.Indices.clear();

            auto addFace = [&](const glm::vec3& normal,
                const glm::vec3& v0,
                const glm::vec3& v1,
                const glm::vec3& v2,
                const glm::vec3& v3)
                {
                    unsigned int baseIndex =
                        static_cast<unsigned int>(mesh.Vertices.size());

                    glm::vec3 tangent = glm::normalize(v1 - v0);
                    glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

                    mesh.Vertices.push_back({ v0, normal, {0,0}, tangent, bitangent });
                    mesh.Vertices.push_back({ v1, normal, {1,0}, tangent, bitangent });
                    mesh.Vertices.push_back({ v2, normal, {1,1}, tangent, bitangent });
                    mesh.Vertices.push_back({ v3, normal, {0,1}, tangent, bitangent });

                    mesh.Indices.insert(mesh.Indices.end(), {
                        baseIndex, baseIndex + 1, baseIndex + 2,
                        baseIndex, baseIndex + 2, baseIndex + 3
                        });
                };

            // Front
            addFace({ 0, 0, 1 }, { -0.5f,-0.5f, 0.5f }, { 0.5f,-0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f });
            // Back
            addFace({ 0, 0,-1 }, { 0.5f,-0.5f,-0.5f }, { -0.5f,-0.5f,-0.5f }, { -0.5f, 0.5f,-0.5f }, { 0.5f, 0.5f,-0.5f });
            // Left
            addFace({ -1, 0, 0 }, { -0.5f,-0.5f,-0.5f }, { -0.5f,-0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f,-0.5f });
            // Right
            addFace({ 1, 0, 0 }, { 0.5f,-0.5f, 0.5f }, { 0.5f,-0.5f,-0.5f }, { 0.5f, 0.5f,-0.5f }, { 0.5f, 0.5f, 0.5f });
            // Top
            addFace({ 0, 1, 0 }, { -0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f,-0.5f }, { -0.5f, 0.5f,-0.5f });
            // Bottom
            addFace({ 0,-1, 0 }, { -0.5f,-0.5f,-0.5f }, { 0.5f,-0.5f,-0.5f }, { 0.5f,-0.5f, 0.5f }, { -0.5f,-0.5f, 0.5f });
        }

        static void CreatePlane(Components::Mesh& mesh) {
            mesh.Vertices.clear();
            mesh.Indices.clear();

            glm::vec3 normal = { 0, 1, 0 };
            glm::vec3 tangent = { 1, 0, 0 };
            glm::vec3 bitangent = { 0, 0, 1 };

            mesh.Vertices.push_back({ { -.5, 0, -.5 }, normal, {0,0}, tangent, bitangent });
            mesh.Vertices.push_back({ {  .5, 0, -.5 }, normal, {1,0}, tangent, bitangent });
            mesh.Vertices.push_back({ {  .5, 0,  .5 }, normal, {1,1}, tangent, bitangent });
            mesh.Vertices.push_back({ { -.5, 0,  .5 }, normal, {0,1}, tangent, bitangent });

            mesh.Indices = { 0, 1, 2, 0, 2, 3 };
        }

        static void CreateSphere(Components::Mesh& mesh, int segments = 32) {
            mesh.Vertices.clear();
            mesh.Indices.clear();

            for (int y = 0; y <= segments; ++y) {
                for (int x = 0; x <= segments; ++x) {
                    float xSeg = (float)x / segments;
                    float ySeg = (float)y / segments;

                    float xPos = std::cos(xSeg * glm::two_pi<float>()) *
                        std::sin(ySeg * glm::pi<float>());
                    float yPos = std::cos(ySeg * glm::pi<float>());
                    float zPos = std::sin(xSeg * glm::two_pi<float>()) *
                        std::sin(ySeg * glm::pi<float>());

                    glm::vec3 pos = { xPos, yPos, zPos };
                    glm::vec3 normal = glm::normalize(pos);

     
                    glm::vec3 tangent = glm::normalize(glm::vec3(
                        -std::sin(xSeg * glm::two_pi<float>()),
                        0.0f,
                        std::cos(xSeg * glm::two_pi<float>())
                    ));
                    glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

                    mesh.Vertices.push_back({
                        pos,
                        normal,
                        { xSeg, ySeg },
                        tangent,
                        bitangent
                        });
                }
            }

            for (int y = 0; y < segments; ++y) {
                for (int x = 0; x < segments; ++x) {
                    unsigned int i0 = y * (segments + 1) + x;
                    unsigned int i1 = (y + 1) * (segments + 1) + x;
                    unsigned int i2 = i0 + 1;
                    unsigned int i3 = i1 + 1;

                    mesh.Indices.insert(mesh.Indices.end(), {
                        i1, i0, i2,
                        i1, i2, i3
                        });
                }
            }
        }


        static void CreateCloth(Components::Mesh& mesh, Components::Cloth& cloth,
            int width, int height, float spacing, glm::mat4 transform, bool isPinned)
        {
  
            mesh.Vertices.clear();
            mesh.Indices.clear();
            cloth.particles.clear();
            cloth.springs.clear();
            mesh.isDynamic = true;

            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {

                    glm::vec3 localPos;
                    glm::vec3 normal;
                    bool applyPin = false;

           
                    if (isPinned) {
                        // Hanging down
                        localPos = glm::vec3(
                            (x - width / 2.0f) * spacing,
                            -(y - height / 2.0f) * spacing, 
                            0.0f
                        );
                        normal = glm::vec3(0, 0, 1);

                        // Pin the top two corners 
                        if (y == 0 && (x == 0 || x == width - 1)) {
                            applyPin = true;
                        }
                    }
                    else {
                        
                        localPos = glm::vec3(
                            (x - width / 2.0f) * spacing,
                            0.0f,                         
                            (y - height / 2.0f) * spacing 
                        );
                        normal = glm::vec3(0, 1, 0); 

         
                        applyPin = false;
                    }

    
                    glm::vec3 worldPos = glm::vec3(transform * glm::vec4(localPos, 1.0f));

    
                    Vertex v;
                    v.Position = worldPos;
                    v.Normal = glm::normalize(glm::vec3(transform * glm::vec4(normal, 0.0f)));
                    v.TexCoords = glm::vec2((float)x / (width - 1), (float)y / (height - 1));
                    mesh.Vertices.push_back(v);

   
                    Components::Cloth::Particle p;
                    p.pos = worldPos;
                    p.oldPos = worldPos;
                    p.isPinned = applyPin;

                    cloth.particles.push_back(p);
                }
            }

 
            for (int y = 0; y < height - 1; ++y) {
                for (int x = 0; x < width - 1; ++x) {
                    int tl = y * width + x;
                    int tr = tl + 1;
                    int bl = (y + 1) * width + x;
                    int br = bl + 1;

                    mesh.Indices.push_back(tl); mesh.Indices.push_back(bl); mesh.Indices.push_back(tr);
                    mesh.Indices.push_back(tr); mesh.Indices.push_back(bl); mesh.Indices.push_back(br);
                }
            }

            //  Springs 

            auto addSpring = [&](int idx1, int idx2, float compliance) {
                Components::Cloth::Spring s;
                s.p1 = idx1;
                s.p2 = idx2;
                s.restLength = glm::length(cloth.particles[idx1].pos - cloth.particles[idx2].pos);
                s.compliance = compliance; 
                cloth.springs.push_back(s);
                };

            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    int i = y * width + x;

              
                    if (x < width - 1) addSpring(i, i + 1, 0.0f);
                    if (y < height - 1) addSpring(i, i + width, 0.0f);

                    // diagnoal
                    if (x < width - 1 && y < height - 1) {
                        addSpring(i, i + width + 1, 0.0f);
                        addSpring(i + 1, i + width, 0.0f);
                    }

                    
                    if (x < width - 2) addSpring(i, i + 2, 1.0f);
                    if (y < height - 2) addSpring(i, i + (width * 2), 1.0f);
                }
            }
        }
    };
}
