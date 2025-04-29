/*
    Handles 
*/
#pragma once

#include "../CrashHandler/CrashHandler.h"
//#include "../Core/Renderer/renderer.h"
//#include "renderApi.h"

#include <string>
#include <fstream>
#include <vector>

namespace Nexus{

    class Shader{
    private:
        std::string fileName;
    public:
        
        Shader(std::string fileName);

        Shader();

        std::vector<char> readShader();
    };
}