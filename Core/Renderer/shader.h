/*
    Handles 
*/
#pragma once

#include "../CrashHandler/CrashHandler.h"

#include <string>
#include <fstream>
#include <vector>

namespace Nexus{
    class Shader{
    private:
        std::string fileName;
    public:
        Shader(std::string fileName);

        std::vector<char> readShader();
    };
}