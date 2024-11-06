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

using namespace Re_Renderer;
int main(){
    

        Window window = Window(800, 600, "Re-Renderer", false);
        Renderer renderer(window);
        CameraSystem cameraSystem(window);

        glViewport(0, 0, window.Width, window.Height);




        double previousTime = glfwGetTime();
        int frameCount = 0;
        Scene scene;
        {
           
            Timer timer("craeting 100000 entities");
            
        
        auto entity = scene.CreateEntity();
        auto transform = entity.addComponent<Components::Transform>();
        transform->setScale(glm::vec3(0.3f));
        transform->setPosition(0,  0 , 0);
        transform->setRotation(0 , 0, 10); 

        entity.addComponent<Components::Mesh>(cubeMesh);
        entity.addComponent<Components::Material>(ShaderType::Basic,glm::vec3(1, 1, 1));

        
         entity = scene.CreateEntity();
         transform = entity.addComponent<Components::Transform>();
        transform->setPosition(0, 0, 1);
        transform->setRotation(0, 0, 0);

        entity.addComponent<Components::Camera>();


        
        }

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







