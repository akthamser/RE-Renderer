#include<iostream>

#include"Renderer/shader.h"
#include"Renderer/Window.h"
#include"Renderer/Renderer.h"
#include"ScreenRecorder.h"
#include"Renderer/Scene.h"


int main(){

    Window window = Window(800, 600,"Re-Renderer",false);
    Renderer renderer = Renderer(window);
    glViewport(0, 0, window.Width, window.Height);

 


    double previousTime = glfwGetTime();
    int frameCount = 0;

    Scene scene;
    scene.CreateEntity();
    int id1 = scene.CreateEntity("father");
    unsigned int id = scene.CreateEntity("car");
    int id2 =scene.CreateEntity("wheel1",id);
    int id3 =scene.CreateEntity("wheel2",id);
 
    scene.makeChild(id,id1);
    scene.makeChild(id2, 0);
   
    scene.PrintHirachy();



    

    while (!window.ShouldClose())
    {
        double currentTime = glfwGetTime();
        frameCount++;

        if (currentTime - previousTime >= 1.0) {
            double fps = double(frameCount) / (currentTime - previousTime);
           // std::cout << "FPS: " << fps << std::endl;

            previousTime = currentTime;
            frameCount = 0;
        }

       // renderer.Render();



        glfwPollEvents();
        window.SwapBuffers();


    }
  
    
    

    glfwTerminate();
    return 0;
}







