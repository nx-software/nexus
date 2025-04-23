#pragma once

#include "transform.h"
#include "../Core/Renderer/shader.h"

#include <string>
#include <vector>

namespace Nexus{
    // All things in a scene extend from this
    class GameObject{
    private:
        int id;
    protected:
        std::string name;
        Transform::Transform transform;
        Shader shader;
    public:
        GameObject(std::string name, Transform::Transform transform);
        GameObject(std::string name);

        void loadShader(std::string file);


        // Setters
        void setId(int id){
            this->id = id;
        }


        // Getters
        int getId(){
            return this->id;
        }

        Shader getShader(){
            return this->shader;
        }


    };
}