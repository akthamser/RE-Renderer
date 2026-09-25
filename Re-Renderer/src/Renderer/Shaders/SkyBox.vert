#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    // Optimization: Z = W ensures the skybox is always at the far plane (1.0 depth)
    gl_Position = pos.xyww; 
}