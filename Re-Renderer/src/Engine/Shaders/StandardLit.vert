#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;


out vec3 FragPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 BiTangent;
out vec2 TexCoords;


uniform mat4 model ;
uniform mat4 view;
uniform mat4 projection;


void main()
{
  
    Normal = aNormal;
    Tangent = aTangent;
    BiTangent = aBiTangent;
    TexCoords= aTexCoords;

    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position =  projection * view * vec4(FragPos, 1.0);;
}