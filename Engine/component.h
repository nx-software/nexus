/*
    Components are modules that are attached to GameObjects
*/
#pragma once

#include <string>

namespace Nexus{
    class Component{
    private:
    protected:
        std::string name;
    public:
        Component(std::string name) : name(name);
    };
}