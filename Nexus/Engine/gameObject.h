/*
    NX-Software Nexus 3D Engine
    gameObject.h - Main class for objects in a scene
    Copyright (C) 2025 Electro-Corp

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "../Core/Renderer/shader.h"
#include "../Core/Renderer/renderer.h"
#include "transform.h"
#include "mesh.h"

#include <string>
#include <vector>

#ifdef WIN32
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllexport)
#endif
#else
#define ENGINE_API
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