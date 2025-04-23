#include "shader.h"

Nexus::Shader::Shader(std::string fileName){
    this->fileName = fileName;
}

Nexus::Shader::Shader(){
    // Do nothing, hope no one calls readShader
}

std::vector<char> Nexus::Shader::readShader(){
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if(!file.is_open()){
        Error(std::string{"Shader file " + fileName + " not found!"});
    }

    size_t fSize = (size_t) file.tellg();
    std::vector<char> buffer(fSize);
    file.seekg(0);
    file.read(buffer.data(), fSize);
    file.close();

    return buffer;
}
