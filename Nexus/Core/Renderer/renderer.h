#pragma once

#ifdef _WIN32
#ifdef RENDERER_EXPORTS
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllexport)
#endif
#else
#define RENDERER_API
#endif

// Includes 
#include "renderApi.h"
#include "Vulkan/vulkanApi.h"
#include "OpenGL/openglApi.h"
// We use Vulkan so
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include <iostream>

namespace Nexus {	
	class GraphicAPI;

	enum RENDERER_API Renderers {
		RENDER_VULKAN,
		RENDER_GL,
		RENDER_DX
	};

	class Renderer {
	private:
		GLFWwindow* window;
		int height, width;

		
		// API
		GraphicAPI* gApi;
		GraphicsCard chosenCard;

		Scene* prev;

		// Stuff to do when kill
		void terminationTasks();

	public:
		// Create Renderer
		RENDERER_API Renderer(std::string title, int height, int width, Renderers render);

		RENDERER_API GraphicAPI* getApi(){ return this->gApi; }

		// Tick
		RENDERER_API void Tick(Scene* scene);

		~Renderer();
	};
}