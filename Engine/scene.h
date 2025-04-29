/*
    Holder for gameObjects
*/
#pragma once

#include <string>
#include <vector>

namespace Nexus{
    class GameObject;

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