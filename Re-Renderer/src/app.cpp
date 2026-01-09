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
#include"Engine/MovmentControllerSystem.h"


using namespace Re_Renderer;
int main(){
    

        Window window = Window(800, 600, "Re-Renderer", false);
        Renderer renderer(window);
        CameraSystem cameraSystem(window);
        AssetsManager assetManager;
        HierarchySystem hierarchySystem;
        MovmentControllerSystem movmentControllerSystem(window);


        glViewport(0, 0, window.Width, window.Height);



        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
       


        Scene scene;
        
           
            
            
        Model& model = assetManager.loadModel("./Assets/Skull/12140_Skull_v3_L2.obj",false);
        EntID skullid = scene.CreateModel(model);
        auto t = scene.getEntityByID(skullid)->getComponent<Components::Transform>();
        t->setPosition(0, 0, 0);
        t->setRotation(-90,0,0);
        t->setScale(0.1,0.1,0.1);

       
       // auto cube = scene.CreateEntity("cube");
       // cube.addComponent<Components::Mesh>(cubeMesh);
       // auto cubeT = cube.getComponent<Components::Transform>();
        
        auto entity = scene.CreateEntity();
        auto transform = entity.getComponent<Components::Transform>();

        transform->setPosition(0, 0, 5);

        transform->setRotation(0, 0, 0);


        entity.addComponent<Components::Camera>();
        entity.addComponent<Components::MovmentController>(15, 15);

        
        
        scene.PrintHierarchy();
        renderer.setupTextures(assetManager);
        renderer.setupScene(scene);



        double previousTime = glfwGetTime(); // to show fps
        double previousFrame = glfwGetTime(); // to calculate deltatime
        int frameCount = 0;

        while (!window.ShouldClose())
        {
            double currentTime = glfwGetTime();
            frameCount++;
            double deltatime = currentTime - previousFrame;
            previousFrame = currentTime; 
             
            if (currentTime - previousTime >= 1.0) {
                double fps = double(frameCount) / (currentTime - previousTime);
                std::cout << "FPS: " << fps << std::endl;

                frameCount = 0;
                previousTime = currentTime;
            }

             t->setPosition(t->getPosition().x, t->getPosition().y - 0.0001 * sin(glfwGetTime()) , t->getPosition().z - 0.0001 * sin(glfwGetTime()));
            

         

            movmentControllerSystem.UpdateMovment(scene, deltatime);

            hierarchySystem.UpdateGlobalTransforms(scene);
            cameraSystem.UpdateCameras(scene);



            renderer.renderScene(scene);


            glfwPollEvents();
            window.SwapBuffers();


        }




        glfwTerminate();
        return 0;
    
}

