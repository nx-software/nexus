# Nexus
## What is Nexus?
Nexus is an open source, 3D (hopefully) cross platform research game engine. <br>
Designed to give developers a starting framework for any project requiring 3D graphics <br>
and/or input, audio control, ect, while still providing full transparency and customization. <br>
## Features
### Rendering APIs
- Vulkan
- OpenGL (skeleton file is present, true support planned)
- DirectX12/11 (still up for debate as to which version, and if to use it at all)
### Sound
- OpenAL support planned
### Input
- Input interface planned
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
