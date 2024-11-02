#include<iostream>

#include"Renderer/shader.h"
#include"Renderer/Window.h"
#include"Renderer/Renderer.h"
#include"ScreenRecorder.h"
#include"Renderer/Scene.h"
#include"config.h"


int main(){
    

        Window window = Window(800, 600, "Re-Renderer", false);
        Re_Renderer::Renderer renderer(window);
        glViewport(0, 0, window.Width, window.Height);




        double previousTime = glfwGetTime();
        int frameCount = 0;





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

            renderer.Render();



            glfwPollEvents();
            window.SwapBuffers();


        }




        glfwTerminate();
        return 0;
    
}







