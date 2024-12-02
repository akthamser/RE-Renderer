#include<iostream>

#include"Engine/shader.h"
#include"Engine/Window.h"
#include"Engine/Renderer.h"
#include"ScreenRecorder.h"
#include"Engine/Scene/Scene.h"
#include"config.h"
#include"Engine/Resources.h"
#include"Engine/Timer.hpp"
#include"Engine/CameraSystem.h"
#include"Engine/AssetsManager.h"
#include"Engine/Model.h"
#include"Engine/HierarchySystem.h"

using namespace Re_Renderer;
int main(){
    

        Window window = Window(800, 600, "Re-Renderer", false);
        Renderer renderer(window);
        CameraSystem cameraSystem(window);
        AssetsManager assetManager;
        HierarchySystem hierarchySystem;

        glViewport(0, 0, window.Width, window.Height);




        double previousTime = glfwGetTime();
        int frameCount = 0;
        Scene scene;
        
           
            
            
            Model& model = assetManager.loadModel("./Assets/Skull/12140_Skull_v3_L2.obj",false);
            EntID skullid = scene.CreateModel(model);
            auto t = scene.getEntityByID(1)->getComponent<Components::Transform>();


       

        
         auto entity = scene.CreateEntity();
         auto transform = entity.getComponent<Components::Transform>();
        transform->setPosition(0, -50, 010);
        transform->setRotation(90, 0, 0);

        entity.addComponent<Components::Camera>();


        
        
        scene.PrintHierarchy();
        renderer.setupTextures(assetManager);
        renderer.setupScene(scene);



        while (!window.ShouldClose())
        {
            double currentTime = glfwGetTime();
            frameCount++;

            if (currentTime - previousTime >= 1.0) {
                double fps = double(frameCount) / (currentTime - previousTime);
                std::cout << "FPS: " << fps << std::endl;

                previousTime = currentTime;
                frameCount = 0;
            }

            t->setPosition(t->getPosition().x, t->getPosition().y - 0.001 * sin(glfwGetTime()) , t->getPosition().z - 0.001 * sin(glfwGetTime()));


            hierarchySystem.UpdateGlobalTransforms(scene);
            cameraSystem.UpdateCameras(scene);
            renderer.renderScene(scene);



            glfwPollEvents();
            window.SwapBuffers();


        }




        glfwTerminate();
        return 0;
    
}







