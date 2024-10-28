#include"Renderer.h"


Renderer::Renderer(Window& window):m_window(window) {

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
   

    m_shader = new Shader("vertexShader.vert","fragmentShader.frag");

    //Quad

    float vertecies[3*4] = {
    1.0f , 1.0f ,0.0f,
    -1.0f , 1.0f ,0.0f,
    1.0f , -1.0f ,0.0f,
    -1.0f , -1.0f ,0.0f
    };

    unsigned int indicies[2*3] = {
        0,1,2,
        1,2,3

    };


    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertecies), vertecies, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindVertexArray(0);

  

}

Renderer::~Renderer() {
    delete m_shader;
}

void Renderer::Render() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader->use();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}