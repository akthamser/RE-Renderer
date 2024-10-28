#include"Window.h"


void Window::window_size_call_back(GLFWwindow* window, int width, int height) {

    Window* win  = (Window*)glfwGetWindowUserPointer(window);

    win->Height = height;
    win->Width = width;
    glViewport(0, 0, width, height);
}

Window::Window(int width,int height,const char* title,bool fullScreen):Width(width),Height(height),Title(title){

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
    

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    void* fullscreen = fullScreen ? monitor : NULL;

    m_window = glfwCreateWindow(width, height, title, (GLFWmonitor*)fullscreen, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }


    glfwMakeContextCurrent(m_window);
 

    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, window_size_call_back);
   
}


Window::~Window() {
    glfwDestroyWindow(m_window);
}

void Window::MakeContextCurrent() {
    glfwMakeContextCurrent(m_window);
}
bool Window::ShouldClose() {
    return glfwWindowShouldClose(m_window);
}
void Window::SwapBuffers() {
    glfwSwapBuffers(m_window);
}


