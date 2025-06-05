/*
    Holder for gameObjects
*/
#pragma once

#include <string>
#include <vector>

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllexport)
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