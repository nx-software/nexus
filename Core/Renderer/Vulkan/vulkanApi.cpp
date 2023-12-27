#include "vulkanApi.h"

Nexus::VulkanAPI::VulkanAPI(GLFWwindow* window) {
	vulkanCreateInstance();
	InitConnectionToWindow(window);
	vulkanDevicePick();
	// Surface needs to exist for this (bad)
	vulkanCreateLogicDev();
}


void Nexus::VulkanAPI::Clean() {
	// Destroy surfaces
	vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);

	vkDestroyDevice(vkDevice, nullptr);

	vkDestroyInstance(vkInstance, nullptr);
	
}

void Nexus::VulkanAPI::InitConnectionToWindow(GLFWwindow* window) {
#ifdef _WIN32
	VkWin32SurfaceCreateInfoKHR crInfo{};
	crInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	crInfo.hwnd = glfwGetWin32Window(window); // get HWND 
	crInfo.hinstance = GetModuleHandle(nullptr);

	// Create surface!
	if (vkCreateWin32SurfaceKHR(vkInstance, &crInfo, nullptr, &vkSurface) != VK_SUCCESS) {
		Error("Vulkan: Failed to create window surface (Win32)!");
	}

	// GLFW should create it now
	if (glfwCreateWindowSurface(vkInstance, window, nullptr, &vkSurface) != VK_SUCCESS) {
		Error("GLFW: Failed to create window surface!");
	}

#endif
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
		Error("Vulkan: Failed to create Vulkan instance!");
	}
}

void Nexus::VulkanAPI::vulkanDevicePick() {
	// Get device count
	uint32_t devCnt = 0;
	vkEnumeratePhysicalDevices(vkInstance, &devCnt, nullptr);

	// Are there any devices at all?
	if (devCnt == 0) {
		// Uh oh, we done here
		Error("Vulkan: No devices that support Vulkan found!");
	}

	std::vector<VkPhysicalDevice> devices(devCnt);
	vkEnumeratePhysicalDevices(vkInstance, &devCnt, devices.data());

	for (const auto& dev : devices) {
		if (isDeviceOk(dev)) {
			vkPhysDevice = dev;
			break;
		}
	}
}

Nexus::QueueFamilyIndicies Nexus::VulkanAPI::findQueueFams(VkPhysicalDevice dev) {
	QueueFamilyIndicies ind;

	uint32_t queueCnt = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueCnt, nullptr);

	std::vector<VkQueueFamilyProperties> queueFam(queueCnt);
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueCnt, queueFam.data());

	

	// Very basic implementation for triangle
	int i = 0; 
	for (const auto& queueF : queueFam) {
		// Queries for draw support (uncomment later and fix)
		VkBool32 presSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, vkSurface, &presSupport);
		if (presSupport) {
			ind.presentFam = i;
		}
		//
		if (queueF.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			ind.graphicsFam = i;
		i++;

		if (ind.isComplete()) break;
	}

	return ind;
}

bool Nexus::VulkanAPI::isDeviceOk(VkPhysicalDevice dev) {
	QueueFamilyIndicies ind = findQueueFams(dev);

	bool swapChainGood = false;
	bool extSupport = checkDevExtSupport(dev);

	if (extSupport) {
		SwapChainSupportDetails scS = getSwapChainSupport(dev);
		swapChainGood = !scS.formats.empty() && !scS.preModes.empty();
	}

	return ind.isComplete() && extSupport && swapChainGood;
}

bool Nexus::VulkanAPI::checkDevExtSupport(VkPhysicalDevice dev) {
	uint32_t extCount;
	vkEnumerateDeviceExtensionProperties(dev, nullptr, &extCount, nullptr);

	// Vector to hold em
	std::vector<VkExtensionProperties> avaExt(extCount);
	vkEnumerateDeviceExtensionProperties(dev, nullptr, &extCount, avaExt.data());

	std::set<std::string> reqExt(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& ext : avaExt) {
		reqExt.erase(ext.extensionName);
	}

	return reqExt.empty();
}

Nexus::SwapChainSupportDetails Nexus::VulkanAPI::getSwapChainSupport(VkPhysicalDevice dev) {
	SwapChainSupportDetails details;

	// Get basic surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, vkSurface, &details.caps);

	// get formats 
	uint32_t formatCnt;
	vkGetPhysicalDeviceSurfaceFormatsKHR(dev, vkSurface, &formatCnt, nullptr);

	if (formatCnt != 0) {
		details.formats.resize(formatCnt);
		vkGetPhysicalDeviceSurfaceFormatsKHR(dev, vkSurface, &formatCnt, details.formats.data());
	}
	else {
		// Log it
	}

	// same thign but with present modes
	uint32_t presModeCnt;
	vkGetPhysicalDeviceSurfacePresentModesKHR(dev, vkSurface, &presModeCnt, nullptr);

	if (formatCnt != 0) {
		details.preModes.resize(presModeCnt);
		vkGetPhysicalDeviceSurfacePresentModesKHR(dev, vkSurface, &presModeCnt, details.preModes.data());
	}
	else {
		// Log it
	}


	return details;
}


void Nexus::VulkanAPI::vulkanCreateLogicDev() {
	QueueFamilyIndicies ind = findQueueFams(vkPhysDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCrInfs;
	std::set<uint32_t> uniqQueueFam = {
		ind.graphicsFam.value(),
		ind.presentFam.value()
	};

	float pri = 1.0f;
	for (uint32_t queueFam : uniqQueueFam) {
		VkDeviceQueueCreateInfo queueInf{};
		queueInf.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		queueInf.queueFamilyIndex = queueFam;
		queueInf.queueCount = 1;
		// Priority
		queueInf.pQueuePriorities = &pri;
		queueCrInfs.push_back(queueInf);
	}

	// Device features
	VkPhysicalDeviceFeatures deviceFeat{};

	// Create logigcal device
	VkDeviceCreateInfo crInf{};
	crInf.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	crInf.pQueueCreateInfos = queueCrInfs.data();
	crInf.queueCreateInfoCount = static_cast<uint32_t>(queueCrInfs.size());

	// Enable extensions
	crInf.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	crInf.ppEnabledExtensionNames = deviceExtensions.data();

	crInf.pEnabledFeatures = &deviceFeat;

	// Validation layers in the future
	crInf.enabledLayerCount = 0; // only used with old drivers

	if (vkCreateDevice(vkPhysDevice, &crInf, nullptr, &vkDevice) != VK_SUCCESS) {
		Error("Vulkan: Failed to create Logical device!");
	}

	

	// Get graphics queue
	vkGetDeviceQueue(vkDevice, ind.graphicsFam.value(), 0, &graphicsQueue);

}