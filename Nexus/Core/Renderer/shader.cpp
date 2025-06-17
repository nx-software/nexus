#include "shader.h"

Nexus::Shader::Shader(std::string source, std::string shader) {
    this->source = source;
    this->shader = shader;
}

Nexus::Shader::Shader(){
    // Do nothing, hope no one calls readShader
}

std::vector<char> Nexus::Shader::readShader(){
    std::ifstream file(shader, std::ios::ate | std::ios::binary);

    if(!file.is_open()){
        Error(std::string{"Shader file " + shader + " not found!"});
    }

    size_t fSize = (size_t) file.tellg();
    std::vector<char> buffer(fSize);
    file.seekg(0);
    file.read(buffer.data(), fSize);
    file.close();

    return buffer;
}

std::vector<char> Nexus::Shader::readSource() {
    std::ifstream file(source, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        Error(std::string{ "Shader Source file " + source + " not found!" });
    }

    size_t fSize = (size_t)file.tellg();
    std::vector<char> buffer(fSize);
    file.seekg(0);
    file.read(buffer.data(), fSize);
    file.close();

    return buffer;
}
