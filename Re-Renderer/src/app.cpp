#include<iostream>

#include"Renderer/shader.h"
#include"Renderer/Window.h"
#include"Renderer/Renderer.h"
#include"ScreenRecorder.h"
#include"Renderer/Scene.h"
#include"config.h"
#include"Renderer/Resources.h"
#include"Renderer/Timer.hpp"

using namespace Re_Renderer;
int main(){
    

        Window window = Window(800, 600, "Re-Renderer", false);
        Renderer renderer(window);
        glViewport(0, 0, window.Width, window.Height);




        double previousTime = glfwGetTime();
        int frameCount = 0;
        Scene scene;
        {
           
            Timer timer("craeting 100000 entities");
            
        for (int i = 0 ; i < 100 ; i++) {
        auto entity = scene.CreateEntity();
        auto transform = entity.addComponent<Components::Transform>();
        transform->Scale = glm::vec3(0.3f);
        transform->Position.x = cos(0.01*i);
        transform->Position.y = sin(0.01 * i);
        transform->Position.z = cos(0.01 * i);
        transform->Rotation.z = cos(0.01 * i);
        entity.addComponent<Components::Mesh>(cubeMesh);
        entity.addComponent<Components::Material>(ShaderType::Basic,glm::vec3(cos(i), sin(i), 0));
        }


        
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

            renderer.renderScene(scene);



            glfwPollEvents();
            window.SwapBuffers();


        }




        glfwTerminate();
        return 0;
    
}







