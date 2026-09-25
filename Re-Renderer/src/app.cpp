#include<iostream>

#include"Renderer/shader.h"
#include"Core/Window.h"
#include"Renderer/Renderer.h"
#include"Utils/ScreenRecorder.h"
#include"ECS/Scene/Scene.h"
#include"ECS/config.h"
#include"Utils/Timer.hpp"
#include"ECS/Systems/CameraSystem.h"
#include"ECS/AssetsManager.h"
#include"ECS/Model.h"
#include"ECS/Systems/HierarchySystem.h"
#include"ECS/Systems/MovmentControllerSystem.h"
#include"Renderer/RayTracer/RayTracer.h"
#include "Core/EditorSystem.h"
#include "ECS/Systems/PhysicsSystem.h"


using namespace Re_Renderer;
int main(){
    

        Window window = Window(800, 600, "Re-Renderer", false);
        Renderer renderer(window);
        CameraSystem cameraSystem(window);
        AssetsManager assetManager;
        HierarchySystem hierarchySystem;
        MovmentControllerSystem movmentControllerSystem(window);
        RayTracer rayTracer(window);

        EditorSystem editor(window.getGLFWwindow());



        glViewport(0, 0, window.Width, window.Height);



        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
       


        Scene scene;
        
           
            
            
        //Model& model = assetManager.loadModel("./Assets/Skull/12140_Skull_v3_L2.obj",false);
        //EntID skullid = scene.CreateModel(model);
        //auto t = scene.getEntityByID(skullid)->getComponent<Components::Transform>();
        //t->setPosition(0, 0, 0);
        //t->setRotation(-90,0,0);
        //t->setScale(.1,.1,.1);

        //std::vector<std::string> faces = {
        //"./Assets/skybox/FluffballDayLeft.hdr",
        //"./Assets/skybox/FluffballDayRight.hdr",
        //"./Assets/skybox/FluffballDayTop.hdr",
        //"./Assets/skybox/FluffballDayBottom.hdr",
        //"./Assets/skybox/FluffballDayFront.hdr",
        //"./Assets/skybox/FluffballDayBack.hdr"
        //};

        std::vector<std::string> faces = {
        "./Assets/maps/px.png",
        "./Assets/maps/nx.png",
        "./Assets/maps/py.png",
        "./Assets/maps/ny.png",
        "./Assets/maps/pz.png",
        "./Assets/maps/nz.png"
        };
     
        scene.SkyboxTextureID = LoadCubemap(faces);


        auto entity = scene.CreateEntity("Camera");
        auto transform = entity.getComponent<Components::Transform>();

        transform->setPosition(0, 0, 5);

        transform->setRotation(0, 0, 0);


        entity.addComponent<Components::Camera>(entity.getID());
        entity.addComponent<Components::MovmentController>(15, 15);




        renderer.setupTextures(assetManager);
        renderer.setupScene(scene);



        double previousTime = glfwGetTime(); // to show fps
        double previousFrame = glfwGetTime(); // to calculate deltatime
        int frameCount = 0;

        double physicsAccumulator = 0.0;
        const double FIXED_DT = 1.0 / 60.0; // We want exactly 60 physics frames per second

        while (!window.ShouldClose())
        {
            double currentTime = glfwGetTime();
            frameCount++;
            double deltatime = currentTime - previousFrame;
            previousFrame = currentTime; 
             
            if (deltatime > 0.25) deltatime = 0.25;

            if (currentTime - previousTime >= 1.0) {
                double fps = double(frameCount) / (currentTime - previousTime);
                std::cout << "FPS: " << fps <<  "  DeltaTime :" << deltatime << std::endl;

                frameCount = 0;
                previousTime = currentTime;
            }

         

            movmentControllerSystem.UpdateMovment(scene, deltatime);

            if (!editor.PausePhysics) {
                // Deposit the real time that passed into our bank
                physicsAccumulator += deltatime;

                // Withdraw time in exactly 60Hz chunks
                while (physicsAccumulator >= FIXED_DT) {

                    // Do your 4 substeps INSIDE this 60Hz chunk
                    int subSteps = 4;
                    float subDt = (float)(FIXED_DT / subSteps);

                    for (int i = 0; i < subSteps; i++) {
                        PhysicsSystem::Update(scene, subDt);
                    }

                    // Deduct the 1/60th of a second we just simulated
                    physicsAccumulator -= FIXED_DT;
                }
            }

            hierarchySystem.UpdateGlobalTransforms(scene);
            cameraSystem.UpdateCameras(scene);
  
            if (editor.useRayTracing) {

                rayTracer.UpdateSceneData(scene);
                rayTracer.Render(scene);
                renderer.resetShader();
            }
            else
                renderer.renderScene(scene);

            editor.Render(scene,assetManager);

            glfwPollEvents();
            window.SwapBuffers();


        }




        glfwTerminate();
        return 0;
    
}

