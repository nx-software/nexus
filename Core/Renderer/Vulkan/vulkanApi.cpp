#include "vulkanApi.h"

Nexus::VulkanAPI::VulkanAPI() {
	vulkanCreateInstance();
	vulkanDevicePick();
}


void Nexus::VulkanAPI::Clean() {
	vkDestroyInstance(vkInstance, nullptr);
}

/*
*	Internal
*/
void Nexus::VulkanAPI::vulkanCreateInstance() {
	VkInstanceCreateInfo crInfo{};
	crInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	// Get extension count
	uint32_t glfwExtCnt = 0;
	const char** glfwExtName;
	glfwExtName = glfwGetRequiredInstanceExtensions(&glfwExtCnt);

	// Give it to crInfo
	crInfo.enabledExtensionCount = glfwExtCnt;
	crInfo.ppEnabledExtensionNames = glfwExtName;

	// Global validation layers
	crInfo.enabledLayerCount = 0;

	// Create
	VkResult result = vkCreateInstance(&crInfo, nullptr, &vkInstance);

	if (result != VK_SUCCESS) {
		// Somethign went wrong (ow)
		Error("Failed to create Vulkan instance!");
	}
}

void Nexus::VulkanAPI::vulkanDevicePick() {
	// Get device count
	uint32_t devCnt = 0;
	vkEnumeratePhysicalDevices(vkInstance, &devCnt, nullptr);

	// Are there any devices at all?
	if (devCnt == 0) {
		// Uh oh, we done here
		Error("No devices that support Vulkan found!");
	}

	std::vector<VkPhysicalDevice> devices;
	vkEnumeratePhysicalDevices(vkInstance, &devCnt, devices.data());
}