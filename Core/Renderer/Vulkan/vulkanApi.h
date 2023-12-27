/*
*	Class for Vulkan rendering
*/
#pragma once



#include <iostream>
#include <vector>
#include <optional>
#include <set>

#include "../renderApi.h"

namespace Nexus {
	// All the Vulkan stuff has queues, so we gotta get them queues
	struct QueueFamilyIndicies {
		std::optional<uint32_t> graphicsFam;
		std::optional<uint32_t> presentFam;

		// we good?
		bool isComplete() {
			return graphicsFam.has_value() && presentFam.has_value();
		}
	};

	class VulkanAPI : public GraphicAPI {
	private:
		// Vulkan
		VkInstance vkInstance;
		VkPhysicalDevice vkPhysDevice;
		VkDevice vkDevice;

		// Queues
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		// Surface
		VkSurfaceKHR vkSurface;

		// Internal Funcs
		// Create VKInstance
		void vulkanCreateInstance();
		// Pick device to use
		void vulkanDevicePick();
		// Create logical device
		void vulkanCreateLogicDev();

		// Find Qeueus
		QueueFamilyIndicies findQueueFams();
	public:
		VulkanAPI();

		void InitConnectionToWindow(GLFWwindow* window) ;
		void Clean();
	};
}