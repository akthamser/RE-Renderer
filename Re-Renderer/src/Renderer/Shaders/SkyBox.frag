#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main() {    
    // Look familiar? It's the same logic as the Ray Tracer!
    // But here we also apply Gamma Correction if your pipeline needs it
    vec3 envColor = texture(skybox, TexCoords).rgb;
    envColor = pow(envColor, vec3(2.2)); // If your texture is sRGB, convert to Linear
    
    FragColor = vec4(envColor, 1.0);
}