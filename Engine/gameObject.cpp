#include "gameObject.h"

Nexus::GameObject::GameObject(std::string name, Transform::Transform transform){
    this->name = name;
    this->transform = transform;
}

Nexus::GameObject::GameObject(std::string name){
    this->name = name;
    this->transform = Transform::Transform(0.0, 0.0, 0.0, 0.0);
}

void Nexus::GameObject::loadShader(std::string file){
    this->shader = Shader(file);
}