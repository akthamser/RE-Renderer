#version 330 core


out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 color = vec3(1,0,1);

void main() {

    FragColor = vec4(vec3(1,0,1), 1.0);   
}