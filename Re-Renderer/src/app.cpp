#include<iostream>

#include"Renderer/shader.h"
#include"Renderer/Window.h"
#include"Renderer/Renderer.h"
#include"ScreenRecorder.h"
#include"Renderer/Scene.h"
#include"config.h"
#include"Renderer/Resources.h"
#include"Renderer/Timer.hpp"
#include"Renderer/CameraSystem.h"
#include"Renderer/AssetsManager.h"
#include"Renderer/Model.h"


using namespace Re_Renderer;
int main(){
    

        Window window = Window(800, 600, "Re-Renderer", false);
        Renderer renderer(window);
        CameraSystem cameraSystem(window);
        AssetsManager assetManager;

        glViewport(0, 0, window.Width, window.Height);




        double previousTime = glfwGetTime();
        int frameCount = 0;
        Scene scene;
        
           
            
            
            Model& model = assetManager.loadModel("./Assets/backpack/backpack.obj",true);
            EntID backbackID = scene.CreateModel(model);



       

        
         auto entity = scene.CreateEntity();
         auto transform = entity.addComponent<Components::Transform>();
        transform->setPosition(7, 0, 0);
        transform->setRotation(0, 90, 0);

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

 
            cameraSystem.UpdateCameras(scene);
            renderer.renderScene(scene);



            glfwPollEvents();
            window.SwapBuffers();


        }




        glfwTerminate();
        return 0;
    
}







