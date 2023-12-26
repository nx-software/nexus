/*
*	Class for Vulkan rendering
*/
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

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
		void vulkanDevicePick();
	public:
		VulkanAPI();

		void Clean();
	};
}