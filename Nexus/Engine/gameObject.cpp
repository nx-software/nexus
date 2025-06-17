#include "gameObject.h"

Nexus::GameObject::GameObject(std::string name, Transform::Transform transform){
    this->name = name;
    this->transform = transform;
}

Nexus::GameObject::GameObject(std::string name){
    this->name = name;
    this->transform = Transform::Transform(0.0, 0.0, 0.0, 0.0);
}

void Nexus::GameObject::loadVertShader(std::string file){
    // Implementation removed because it is unnessacry for now.
    //this->vertShader = new Shader(file);
}

void Nexus::GameObject::loadFragShader(std::string file){
    // Implementation removed because it is unnessacry for now.
    //this->fragShader = new Shader(file);
}