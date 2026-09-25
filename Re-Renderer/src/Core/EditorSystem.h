#pragma once
#include "../ECS/Scene/Scene.h"
#include "../ECS/Components/Components.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h> 


#include "../ECS/PrimitiveGenerator.h" 
#include "../ECS/AssetsManager.h" 

namespace Re_Renderer {

    class EditorSystem {
    public:
        EditorSystem(GLFWwindow* window) {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui::StyleColorsDark();

            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 430");
        }

        ~EditorSystem() {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        void Render(Scene& scene, AssetsManager& assetManager) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            DrawHierarchy(scene, assetManager);
            DrawInspector(scene);
            DrawPerformanceOverlay();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        bool useRayTracing = true;
        bool PausePhysics = false;

    private:
        EntID m_SelectedEntity = 0;


        bool openImportModal = false;
        char importPathBuffer[256] = "./Assets/Skull/12140_Skull_v3_L2.obj";

    
        bool openClothModal = false;
        int clothWidth = 30;
        int clothHeight = 30;
        float clothSpacing = 0.075f;
        bool clothIsPinned = false;

        void DrawHierarchy(Scene& scene, AssetsManager& assetManager) {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x * 0.2f, viewport->WorkSize.y));

            ImGui::Begin("Scene Hierarchy", nullptr, ImGuiWindowFlags_NoResize);

    
            for (auto& entity : scene.Entities) {
                if (entity.getParentID() == 0) {
                    DrawEntityNode(entity, scene);
                }
            }


            if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight))
            {
                //Empty Entity
                if (ImGui::MenuItem("Create Empty Entity")) {
                    auto& e = scene.CreateEntity("Empty Entity");
                    e.addComponent<Components::Transform>();
                }

                ImGui::Separator();

                //Primitives
                if (ImGui::MenuItem("Create Cube")) {
                    auto& e = scene.CreateEntity("Cube");
                    e.addComponent<Components::Transform>();
                    auto* m = e.addComponent<Components::Mesh>();
                    auto* mat = e.addComponent<Components::Material>();
                    mat->BaseColor = glm::vec3(1.0f);
                    mat->shaderType = ShaderType::Blin_Phong;
                    PrimitiveGenerator::CreateCube(*m);
                }

                if (ImGui::MenuItem("Create Sphere")) {
                    auto& e = scene.CreateEntity("Sphere");
                    e.addComponent<Components::Transform>();
                    auto* m = e.addComponent<Components::Mesh>();
                    auto* mat = e.addComponent<Components::Material>();
                    mat->BaseColor = glm::vec3(1.0f);
                    mat->shaderType = ShaderType::Blin_Phong;
                    PrimitiveGenerator::CreateSphere(*m);
                }

                if (ImGui::MenuItem("Create Plane")) {
                    auto& e = scene.CreateEntity("Plane");
                    e.addComponent<Components::Transform>();
                    auto* m = e.addComponent<Components::Mesh>();
                    auto* mat = e.addComponent<Components::Material>();
                    mat->BaseColor = glm::vec3(1.0f);
                    mat->shaderType = ShaderType::Blin_Phong;
                    PrimitiveGenerator::CreatePlane(*m);
                }

                if (ImGui::MenuItem("Create Cloth")) {
                    openClothModal = true;
                }

                ImGui::Separator();

                //Import Model 
                if (ImGui::MenuItem("Import OBJ Model...")) {
                    openImportModal = true;
                }

                ImGui::Separator();

                if (ImGui::BeginMenu("Create Light"))
                {
                    if (ImGui::MenuItem("Point Light")) {
                        auto& e = scene.CreateEntity("Point Light");
                        e.addComponent<Components::Transform>();
                        e.addComponent<Components::Light>(LightType::Point, glm::vec3(1.0f), 1.0f, 10.0f);
                    }

                    if (ImGui::MenuItem("Directional Light")) {
                        auto& e = scene.CreateEntity("Directional Light");
                        e.addComponent<Components::Transform>();
                        e.addComponent<Components::Light>(LightType::Directional, glm::vec3(1.0f), 1.0f, 0.0f);
                    }

                    if (ImGui::MenuItem("Spot Light")) {
                        auto& e = scene.CreateEntity("Spot Light");
                        e.addComponent<Components::Transform>();
                        e.addComponent<Components::Light>(LightType::Spot, glm::vec3(1.0f), 5.0f, 20.0f);
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndPopup();
            }

            ImGui::End();

   
            DrawImportModal(scene, assetManager);
            DrawClothModal(scene);
        }

        void DrawClothModal(Scene& scene) {
            if (openClothModal) {
                ImGui::OpenPopup("Create Cloth Settings");
                openClothModal = false;
            }

            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Create Cloth Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

                ImGui::InputInt("Width (Particles)", &clothWidth);
                ImGui::InputInt("Height (Particles)", &clothHeight);
                ImGui::InputFloat("Spacing", &clothSpacing, 0.01f, 0.1f, "%.3f");
                ImGui::Checkbox("Pin Top Corners (Hang vertically)", &clothIsPinned);

                if (clothWidth < 2) clothWidth = 2;
                if (clothHeight < 2) clothHeight = 2;
                if (clothSpacing < 0.01f) clothSpacing = 0.01f;

                ImGui::Separator();

                if (ImGui::Button("Create", ImVec2(120, 0))) {
                    auto& e = scene.CreateEntity("Cloth");

                    auto* t = e.addComponent<Components::Transform>();
                    t->setPosition(glm::vec3(0, 5, 0));

                    auto* m = e.addComponent<Components::Mesh>();
                    auto* c = e.addComponent<Components::Cloth>();
                    auto* mat = e.addComponent<Components::Material>();
                    mat->shaderType = ShaderType::Blin_Phong;
                    mat->BaseColor = glm::vec3(0.8f, 0.2f, 0.2f);
                    mat->Roughness = 0.8f;

                    PrimitiveGenerator::CreateCloth(*m, *c, clothWidth, clothHeight, clothSpacing, t->getModel(), clothIsPinned);

                    t->setPosition(glm::vec3(0, 0, 0));
                    t->setRotation(glm::vec3(0, 0, 0));
                    t->setScale(glm::vec3(1, 1, 1));

                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        void DrawImportModal(Scene& scene, AssetsManager& assetManager) {
            if (openImportModal) {
                ImGui::OpenPopup("Import Model");
                openImportModal = false;
            }

            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Import Model", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Enter OBJ file path:");
                ImGui::InputText("Path", importPathBuffer, 256);

                ImGui::Separator();

                if (ImGui::Button("Load", ImVec2(120, 0))) {
                    try {
                        std::cout << "Attempting to load: " << importPathBuffer << std::endl;
                        Model& model = assetManager.loadModel(importPathBuffer, false);
                        scene.CreateModel(model);
                        std::cout << "Success!" << std::endl;
                    }
                    catch (...) {
                        std::cout << "Failed to load model. Check path." << std::endl;
                    }
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        void DrawEntityNode(Entity& entity, Scene& scene) {
            ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity.getID()) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

            if (entity.getChildrenIDs().empty()) {
                flags |= ImGuiTreeNodeFlags_Leaf;
            }

            bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity.getID(), flags, entity.getName().c_str());

            if (ImGui::IsItemClicked()) {
                m_SelectedEntity = entity.getID();
            }

            if (opened) {
                for (EntID childID : entity.getChildrenIDs()) {
                    Entity* child = scene.getEntityByID(childID);
                    if (child) DrawEntityNode(*child, scene);
                }
                ImGui::TreePop();
            }
        }

        void DrawInspector(Scene& scene) {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            float width = viewport->WorkSize.x * 0.25f;
            ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - width, viewport->WorkPos.y));
            ImGui::SetNextWindowSize(ImVec2(width, viewport->WorkSize.y));

            ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

            if (m_SelectedEntity != 0) {
                Entity* entity = scene.getEntityByID(m_SelectedEntity);
                if (entity) {
                    ImGui::Text("ID: %d", entity->getID());

                    char buffer[256];
#ifdef _MSC_VER
                    strncpy_s(buffer, entity->getName().c_str(), sizeof(buffer));
#else
                    strncpy(buffer, entity->getName().c_str(), sizeof(buffer));
#endif

                    if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
                        std::string newName(buffer);
                        entity->setName(newName);
                    }

                    ImGui::Separator();

                    // DELETE

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
                    if (ImGui::Button("Delete Entity", ImVec2(-1, 0))) {
                        scene.RemoveEntity(m_SelectedEntity);
                        m_SelectedEntity = 0;
                        ImGui::PopStyleColor(3);
                        ImGui::End();
                        return; 
                    }
                    ImGui::PopStyleColor(3);

                    ImGui::Separator();


                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
                    if (ImGui::Button("Add Component", ImVec2(-1, 0))) {
                        ImGui::OpenPopup("AddComponentPopup");
                    }
                    ImGui::PopStyleColor();

                    if (ImGui::BeginPopup("AddComponentPopup")) {

                        if (!entity->getComponent<Components::Collider>()) {
                            if (ImGui::MenuItem("Collider")) {
                                entity->addComponent<Components::Collider>();
                                ImGui::CloseCurrentPopup();
                            }
                        }
                        else {
                            ImGui::TextDisabled("Entity already has a Collider");
                        }

                        ImGui::EndPopup();
                    }

                    ImGui::Separator();

                    // COMPONENT INSPECTORS 

                    auto transform = entity->getComponent<Components::Transform>();
                    if (transform) {
                        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                            glm::vec3 pos = transform->getLocalPosition();
                            glm::vec3 rot = glm::degrees(transform->getlocalRotation());
                            glm::vec3 scale = transform->getLocalScale();

                            if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) transform->setPosition(pos);
                            if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f)) transform->setRotation(rot);
                            if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
                                if (abs(scale.x) < 0.001f) scale.x = 0.001f;
                                if (abs(scale.y) < 0.001f) scale.y = 0.001f;
                                if (abs(scale.z) < 0.001f) scale.z = 0.001f;
                                transform->setScale(scale);
                            }
                        }
                    }

                    auto collider = entity->getComponent<Components::Collider>();
                    if (collider) {
                        if (ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
                            const char* types[] = { "Sphere", "Plane", "Box" };
                            int current = (int)collider->type;
                            if (ImGui::Combo("Type", &current, types, 3)) {
                                collider->type = (Components::Collider::Type)current;
                            }

                            ImGui::DragFloat3("Offset", &collider->offset.x, 0.1f);

                            if (collider->type == Components::Collider::SPHERE) {
                                ImGui::DragFloat("Radius", &collider->radius, 0.1f);
                            }
                            else if (collider->type == Components::Collider::PLANE) {
                                ImGui::DragFloat("Plane Height", &collider->planeHeight, 0.1f);
                            }
                            else if (collider->type == Components::Collider::BOX) {
                                ImGui::DragFloat3("Box Size", &collider->boxSize.x, 0.1f);
                            }
                        }
                    }

                    auto cloth = entity->getComponent<Components::Cloth>();
                    if (cloth) {
                        if (ImGui::CollapsingHeader("Cloth Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::Text("Particles: %zu", cloth->particles.size());
                        }
                    }

                    auto camera = entity->getComponent<Components::Camera>();
                    if (camera) {
                        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::Text("Active Camera");
                            float fov = camera->getFOV();
                            if (ImGui::DragFloat("FOV", &fov, 1.0f, 10.0f, 180.0f)) camera->setFOV(fov);
                            float farPlane = camera->getFarClippingPlane();
                            if (ImGui::DragFloat("Far Clipping Plane", &farPlane, 1.0f, 10.0f, 100.0f)) camera->setFarClippingPlane(farPlane);
                        }
                    }

                    auto material = entity->getComponent<Components::Material>();
                    if (material) {
                        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::ColorEdit3("BaseColor", &material->BaseColor.x);
                            ImGui::DragFloat("Roughness", &material->Roughness, 0.01f, 0.01f, 1.0f);
                            ImGui::DragFloat("Metallic", &material->Metallic, 0.01f, 0.0f, 1.0f);
                        }
                    }

                    auto light = entity->getComponent<Components::Light>();
                    if (light) {
                        if (ImGui::CollapsingHeader("Light Component", ImGuiTreeNodeFlags_DefaultOpen)) {
                            const char* lightTypes[] = { "Directional", "Point", "Spot" };
                            int currentType = (int)light->type;
                            if (ImGui::Combo("Type", &currentType, lightTypes, IM_ARRAYSIZE(lightTypes))) {
                                light->type = (LightType)currentType;
                                light->isDirty = true;
                            }

                            if (ImGui::ColorEdit3("Color", &light->color.x)) light->isDirty = true;
                            if (ImGui::DragFloat("Intensity", &light->intensity, 0.1f, 0.0f, 1000.0f)) light->isDirty = true;

                            if (light->type != LightType::Directional) {
                                if (ImGui::DragFloat("Range", &light->radius, 0.5f, 0.0f, 1000.0f)) light->isDirty = true;
                            }

                            if (light->type == LightType::Spot) {
                                ImGui::Separator();
                                ImGui::Text("Spotlight Settings");
                                if (ImGui::DragFloat("Inner Cutoff (Deg)", &light->innerCutoff, 0.5f, 0.0f, 90.0f)) light->isDirty = true;
                                if (ImGui::DragFloat("Outer Cutoff (Deg)", &light->outerCutoff, 0.5f, 0.0f, 90.0f)) light->isDirty = true;
                                if (light->innerCutoff > light->outerCutoff) light->innerCutoff = light->outerCutoff;
                            }
                        }
                    }

                    auto mesh = entity->getComponent<Components::Mesh>();
                    if (mesh) {
                        if (ImGui::CollapsingHeader("Mesh")) {
                            ImGui::Text("Mesh Component Attached");
                            ImGui::Text("Vertices: %zu", mesh->Vertices.size());
                            ImGui::Text("Indices: %zu", mesh->Indices.size());
                        }
                    }

                    auto move = entity->getComponent<Components::MovmentController>();
                    if (move) {
                        if (ImGui::CollapsingHeader("Movement Controller")) {
                            ImGui::DragFloat("Speed", &move->speed);
                            ImGui::DragFloat("Sensitivity", &move->sensitivity);
                        }
                    }

                }
            }
            else {
                ImGui::Text("Select an entity to view details.");
            }
            ImGui::End();
        }

        void DrawPerformanceOverlay() {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + (viewport->WorkSize.x * 0.2f) + 10.0f, viewport->WorkPos.y + 10.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowBgAlpha(0.35f);

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoFocusOnAppearing |
                ImGuiWindowFlags_NoNav |
                ImGuiWindowFlags_NoCollapse;

            if (ImGui::Begin("Performance Overlay", nullptr, window_flags)) {
                ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);

                ImGui::Separator();
                ImGui::Checkbox("Enable Ray Tracing", &useRayTracing);
                ImGui::Checkbox("Pause Physics", &PausePhysics);
            }
            ImGui::End();
        }
    };
}