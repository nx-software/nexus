#pragma once

// Includes 
#include "renderApi.h"
#include "vulkan/vulkanApi.h"
// We use Vulkan so
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include <iostream>

namespace Nexus {
	/*
	* The engine is cross platform (or it should be at least)
	* so it uses Vulkan and GLFW. 
	*/
	class Renderer {
	private:
		GLFWwindow* window;
		int height, width;

		
		// API
		GraphicAPI* gApi;

		
		// Stuff to do when kill
		void terminationTasks();
		// Pick a device
		void pickDevice();

	public:
		// Create Renderer
		Renderer(std::string title, int height, int width);

		// Tick
		void Tick();

		~Renderer();
	};
}