#pragma once

namespace Re_Renderer {

    struct ShaderPath {

        std::string vertexPath;
        std::string fragmentPath;

    };

    enum class ShaderType {
        Basic,           // Basic shader without any lighting

        Count            // Keep track of the number of shader types
    };

    static std::vector<ShaderPath> shaderPaths = {
        {"vertexShader.vert","fragmentShader.frag"},// Basic



    };

}