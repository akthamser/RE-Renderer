#include<iostream>

#include"Renderer/shader.h"
#include"Renderer/Window.h"
#include"Renderer/Renderer.h"
#include"ScreenRecorder.h"


int main(){

    Window window = Window(800, 600,"Re-Renderer",false);
    Renderer renderer = Renderer(window);
    glViewport(0, 0, window.Width, window.Height);

    bool recorde = false;
    ScreenRecorder screenRecorder = ScreenRecorder(window.Width, window.Height,60,"Recordings/video.mp4",false);

    if (recorde)
        screenRecorder.StartRecording();

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

      if(recorde)
        screenRecorder.captureFrame();

        glfwPollEvents();
        window.SwapBuffers();


    }
  
    
    

    glfwTerminate();
    return 0;
}







