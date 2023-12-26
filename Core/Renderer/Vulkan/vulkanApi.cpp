#include "vulkanApi.h"

Nexus::VulkanAPI::VulkanAPI() {
	vulkanCreateInstance();
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
		printf("[Nexus] Fatal: Failed to create Vulkan instance! Press ENTER to terminate!\n");
		getchar();
		exit(-1);
	}
}