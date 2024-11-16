#pragma once
#include<string>

namespace Re_Renderer {

    struct ShaderPath {

        std::string vertexPath;
        std::string fragmentPath;

    };

    enum class ShaderType {
        Basic,           // Basic shader without any lighting
        Blin_Phong,
        Count            // Keep track of the number of shader types
    };

    static std::vector<ShaderPath> shaderPaths = {
        {"vertexShader.vert","fragmentShader.frag"},// Basic
        {"vertexShader - Copy.vert","fragmentShader - Copy.frag"},// Basic



    };

}