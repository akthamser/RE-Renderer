#pragma once
#include <glad/glad.h> 
#include<glm/glm.hpp>
#include"../Utils/Utils.h"


constexpr const char* SHADER_PATH = "./src/Renderer/Shaders/";

class Shader
{
public:
    unsigned int ID;


    Shader() = delete;

    Shader(const char* vertexFile, const char* fragmentFile);
    ~Shader();

    void use();

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec3(const std::string& name, glm::vec3 vec) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;


};