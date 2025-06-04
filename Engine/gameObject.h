#pragma once

#include "transform.h"
#include "mesh.h"
#include "../Core/Renderer/shader.h"
#include "../Core/Renderer/renderer.h"

#include <string>
#include <vector>

namespace Nexus{
    //class GraphicsShader;
    // All things in a scene extend from this
    class GameObject{
    private:
        int id;
    protected:
        std::string name;
        Transform::Transform transform;
        Shader vertShader, fragShader;
    public:
        GameObject(std::string name, Transform::Transform transform);
        GameObject(std::string name);

        void loadVertShader(std::string file);
        void loadFragShader(std::string file);

        // For the graphics APIs
        GraphicsShader* gShader;

        // Our current mesh
        Mesh* mesh = nullptr;

        // Setters
        void setId(int id){
            this->id = id;
        }

        // Getters
        int getId(){
            return this->id;
        }

        Shader getVertShader(){
            return this->vertShader;
        }

        Shader getFragShader(){
            return this->fragShader;
        }


    };
}