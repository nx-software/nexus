/*
    NX-Software Nexus 3D Engine
    scene.h - Glorified holder for GameObjects
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

#include <string>
#include <vector>

#ifdef _WIN32
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllexport)
#endif
#else
#define ENGINE_API
#endif

namespace Nexus{
    class GameObject;

    class ENGINE_API Scene{
    private:
        // Scene name
        std::string name;
        // Gameobjects 
        std::vector<GameObject*> objects;
    public:
        Scene(std::string name);

        void addObject(GameObject* object);

        std::vector<GameObject*> getObjects(){
            return objects;
        }
    };
}