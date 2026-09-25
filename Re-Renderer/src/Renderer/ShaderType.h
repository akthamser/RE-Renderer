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
        SkyBox = 3,
        Count 

    };

    static std::vector<ShaderPath> shaderPaths = {
        {"vertexShader.vert","fragmentShader.frag"},// Basic
        {"StandardLit.vert","StandardLit.frag"},// Blin_phong
        {"vertexShader - Copy.vert","fragmentShader - Copy.frag"},// Basic
        {"SkyBox.vert","SkyBox.frag"},// SkyBox



    };

}