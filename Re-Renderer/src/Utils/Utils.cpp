#include"Utils.h"

std::string ReadShaderfromFile(std::string shaderPath) {
    std::string shaderCode;
    std::ifstream ShaderFile;

    // ensure ifstream objects can throw exceptions:

    ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        
        // open file
        ShaderFile.open(shaderPath);  
        std::stringstream ShaderStream;
        // read file's buffer contents into streams
        ShaderStream << ShaderFile.rdbuf();
        // close file handlers
        ShaderFile.close();
        // convert stream into string
        shaderCode = ShaderStream.str(); 
  
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    return shaderCode;
}

