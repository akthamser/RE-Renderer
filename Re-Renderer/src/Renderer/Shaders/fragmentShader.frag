#version 330 core


out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 color = vec3(1,1,1);

void main() {

    FragColor = vec4(color, 1.0);   
}