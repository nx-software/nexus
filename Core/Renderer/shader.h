/*
    Handles 
*/
#pragma once

#include "../CrashHandler/CrashHandler.h"
#include "../Core/Renderer/renderer.h"

#include <string>
#include <fstream>
#include <vector>

namespace Nexus{
    class Shader{
    private:
        std::string fileName;
    public:
        // For the graphics APIs
        GraphicsShader* gShader;

        Shader(std::string fileName);

        Shader();

        std::vector<char> readShader();
    };
}