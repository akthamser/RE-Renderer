#version 330 core


out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Tangent;
in vec3 BiTangent;


struct Material {
    sampler2D Ambient;
    sampler2D Diffuse;       
    sampler2D NormalMap;     
    sampler2D SpecularMap;   
};

uniform Material material; 

uniform vec3 color = vec3(1,0,1);

void main() {

    vec3 diffuseColor = vec3(texture(material.Diffuse, TexCoords));
    FragColor = vec4(diffuseColor,1.0);   
}