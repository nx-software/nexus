/*
    Holder for gameObjects
*/
#pragma once

#include "gameObject.h"

#include <vector>

namespace Nexus{
    class Scene{
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