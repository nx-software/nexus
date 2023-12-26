/*
*	Class for Vulkan rendering
*/
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

#include "../renderApi.h"

namespace Nexus {
	class VulkanAPI : public GraphicAPI {
	private:
		// Vulkan
		VkInstance vkInstance;
		VkPhysicalDevice vkDevice;

		// Internal Funcs
		// Create VKInstance
		void vulkanCreateInstance();
	public:
		VulkanAPI();

		void Clean();
	};
}