# Nexus
(Hopefully) Cross platform game engine. <br>
Currently supports Vulkan, may be expanded to support DX12. <br>
OpenGL skeleton file is in there, will be added in the future.
## File structure
### Core
Contains the core of the engine (eg. Rendering, Input, Audio)
### Engine 
Contains the actual Engine part (eg. Physics, manages/creates graphics, scripting)
### TestProgram
Test program showing off the capabilities of the engine

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
Ever since I converted this stuff into 3 DLLs I haven't tested it out on Linux.

## TO DO LIST:
- [ ] - [ ] `VulkanAPI` : Optimize buffers 
- [ ] - [ ] `Core/Rendering/Vulkan/vulkanAPI.cpp` : Check that Graphics Cards are compatible with program
