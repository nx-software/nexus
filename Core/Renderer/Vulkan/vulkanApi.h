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

	// Swap chains may not be compatible with our surface,
	// so lets make sure it has the stuff we need
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR caps;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> preModes;
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
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

		// All these functions have a VkPhysicalDevice as a prarm
		// because we check multiple devices in order to choose the one
		// we set as our vkPhysDevice. 

		// Check if the device is good
		bool isDeviceOk(VkPhysicalDevice dev);

		bool checkDevExtSupport(VkPhysicalDevice dev);

		// Find Qeueus
		QueueFamilyIndicies findQueueFams(VkPhysicalDevice dev);
		// Get details about swap chains
		SwapChainSupportDetails getSwapChainSupport(VkPhysicalDevice device);
	public:
		VulkanAPI(GLFWwindow* window);

		void InitConnectionToWindow(GLFWwindow* window) ;
		void Clean();
	};
}