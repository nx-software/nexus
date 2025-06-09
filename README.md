# Nexus
## What is Nexus?
Nexus is an open source, 3D (hopefully) cross platform research game engine. <br>
Designed to give developers a starting framework for any project requiring 3D graphics <br>
and/or input, audio control, ect, while still providing full transparency and customization. <br>
Nexus was also designed to be modular, as anything besides Rendering such as Audio, Input, ect is an optional <br>
feature that one can simpily choose to not load.
## Features
### Rendering APIs
- Vulkan
- OpenGL (skeleton file is present, true support planned)
- DirectX12/11 (still up for debate as to which version, and if to use it at all)
### Sound
- OpenAL support planned
### Input
- Input interface planned

## Modules
Anything that isn't Rendering is an optional module that can be chosen to be loaded in at runtime. 
Eg, audio:
```c++
// Load audio module
Nexus::Module audio("Audio.dll");
audio.initClass("Audio");
audio.runFunction("Audio", "initAudio");
```
### Creating a module 
#### Windows
Create a new VS DLL project. Disable precompiled headers if you want, and make your DLLs and classes as normal. <br>
Before we can have Nexus's module loader use it, we need to do two things. <br>
First, lets setup a way for Nexus to create an instance of the class:
```c++
extern "C" __declspec(dllexport) YourNamespace::YourClass* createClass(){
  return new YourNamespace::YourClass();
}
```
This function **MUST** be called `createClass`. <br>
Next, to actually be able to call a function, we must make a C style function for each class function:
```c++
// Assume ReturnType YourNamespace::YourClass::FooBar() is a function
extern "C" __declspec(dllexport) ReturnType FooBar(YourNamespace::YourClass* instance){
  return instance->FooBar();
}
```
Do this for each function you want to call from Nexus.
## File structure
### Core
Contains the core of the engine (eg. Rendering, Input, Audio)
### Engine 
Contains the actual Engine part (eg. Physics, manages/creates graphics, scripting)
### TestProgram
Test program showing off the capabilities of the engine

## Third party libraries
- Vulkan SDK
- GLFW
- glm (included in `thirdpartyinc`)
- plog (included in `thirdpartyinc`)
- stb_image (included in `thirdpartyinc`)

## Output details
### Windows
On Windows, each component (Rendering, Input, Audio, Engine) is compiled into a DLL. <br>
Currently, the output should look like this:
```
- x64
  - Nexus.exe
  - Renderer.dll
  - Engine.dll
  - CrashHandler.dll
```

### Linux
Ever since I converted this stuff into 3 DLLs I haven't tested it out on Linux. <br> 
I imagine the CMake file should compile everything into a single ELF though.

## TO DO LIST:
- [ ] - [ ] `VulkanAPI` : Optimize buffers 
- [ ] - [ ] `Core/Rendering/Vulkan/vulkanAPI.cpp` : Check that Graphics Cards are compatible with program
