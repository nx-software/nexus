#pragma once

#include "../Core/Renderer/shader.h"
#include "transform.h"
#include "mesh.h"
#include "../Core/Renderer/renderer.h"

#include <string>
#include <vector>

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllexport)
#endif

namespace Nexus{
    class Shader;
    // All things in a scene extend from this
    class ENGINE_API GameObject{
    private:
        int id;
    protected:
        std::string name;
        Transform::Transform transform;
        Shader* vertShader;
        Shader* fragShader;
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

        void setVertShader(Shader* s) {
            this->vertShader = s;
        }

        void setFragShader(Shader* s) {
            this->fragShader = s;
        }

        // Getters
        int getId(){
            return this->id;
        }

        Shader* getVertShader(){
            return this->vertShader;
        }

        Shader* getFragShader(){
            return this->fragShader;
        }

        std::string getName() {
            return name;
        }


    };
}