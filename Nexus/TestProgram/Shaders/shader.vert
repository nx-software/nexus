#version 450

layout(binding = 0) uniform CameraData {
    mat4 model;
    mat4 view;
    mat4 proj;
} camData;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main(){
    gl_Position = camData.proj * camData.view * camData.model * vec4(inPos, 0.0, 1.0);
    fragColor = inColor;
}