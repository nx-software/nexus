#include "transform.h"

Transform::Vector2::Vector2(){
    x = 0;
    y = 0;
}

Transform::Vector2::Vector2(float x, float y) : x(x), y(y){

}

Transform::Vector3::Vector3(){
    x = 0;
    y = 0;
    z = 0;
}

Transform::Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z){

}

Transform::Transform::Transform(){
    this->angle = 0;
    this->scale.x = 1;
    this->scale.y = 1;
}

Transform::Transform::Transform(float x, float y, float z, float ang){
    this->position.x = x;
    this->position.y = y;
    this->position.z = z;
    this->angle = ang;
    this->scale.x = 1;
    this->scale.y = 1;
}

Transform::Transform::Transform(Vector3 pos, float ang){
    this->position = pos;
    this->angle = ang;
    this->scale.x = 1;
    this->scale.y = 1;
}

Transform::Vector2 Transform::Vector2::operator+(Vector2 other){
    // Add the vectors
    return Vector2(this->x + other.x, this->y + other.y);
}

Transform::Vector3 Transform::Vector3::operator+(Vector3 other){
    // Add the vectors
    return Vector3(this->x + other.x, this->y + other.y, this->z + other.z);
}