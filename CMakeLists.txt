cmake_minimum_required(VERSION 3.15)
project(Nexus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

set(BUILD_WSI_XLIB_SUPPORT OFF)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Core/Renderer
    ${CMAKE_SOURCE_DIR}/Core/Renderer/Vulkan
    ${CMAKE_SOURCE_DIR}/CrashHandler
    ${CMAKE_SOURCE_DIR}/Engine
    ${CMAKE_SOURCE_DIR}/thirdPartyInc
)

# Source files
set(SOURCES
    Core/Renderer/renderer.cpp
    Core/Renderer/renderApi.cpp
    Core/Renderer/Vulkan/vulkanApi.cpp
    Core/Renderer/OpenGL/openglApi.cpp
    Core/Renderer/shader.cpp
    Engine/nexus.cpp
    Engine/transform.cpp
    Engine/gameObject.cpp
    Engine/scene.cpp
    CrashHandler/CrashHandler.cpp
    TestProgram/main.cpp

)

# Add executable
add_executable(Nexus ${SOURCES})

# Link libraries (LDFLAGS)
target_link_libraries(Nexus
    glfw
    vulkan
    dl
    pthread
    X11
    Xxf86vm
    Xrandr
    Xi
    xcb
    X11-xcb
)
