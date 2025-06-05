#include "scene.h"

Nexus::Scene::Scene(std::string){
    this->name = name;
}

void Nexus::Scene::addObject(GameObject* object){
    objects.push_back(object);
}
