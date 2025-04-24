/*
*	Graphics rendering via the Vulkan Graphics API
*/

#include "vulkanApi.h"

Nexus::VulkanAPI::VulkanAPI(GLFWwindow* window) {
	// Set defaults
#ifndef __ANDROID__
#ifdef _WIN32 || __linux__ || mac
	glfwGetFramebufferSize(window, &width, &height);
#endif
	// Nice tradeoff (avoid tearing, low latency) 
	vkPreferedSwapPresentationMode = VK_PRESENT_MODE_MAILBOX_KHR;
#else 
	// If energy is a concern (ie. on mobile devices), use FIFO
	vkPreferedSwapPresentationMode = VK_PRESENT_MODE_FIFO_KHR;
#endif

	vulkanCreateInstance();
	InitConnectionToWindow(window);
	vulkanDevicePick();
	// Surface needs to exist for this (bad)
	vulkanCreateLogicDev();
	// Create swap chain
	vulkanCreateSwapChain();
	// Create swap chain images
	vulkanCreateImageViews();
	// Create graphics pipeline
	vulkanCreateGraphicsPipeline();
}


void Nexus::VulkanAPI::Clean() {
	// Destroy image views
	for (auto imageView : vkSwapChainImgViews) {
		vkDestroyImageView(vkDevice, imageView, nullptr);
	}
	// Destroy swap chain
	vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
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
#ifdef __linux__
	VkXcbSurfaceCreateInfoKHR crInfo{};
	crInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	crInfo.flags = 0;
	crInfo.pNext = NULL;
	crInfo.connection = XGetXCBConnection(glfwGetX11Display());
	crInfo.window = glfwGetX11Window(window);
	if (vkCreateXcbSurfaceKHR(vkInstance, &crInfo, nullptr, &vkSurface) != VK_SUCCESS){
		Error("Vulkan: Failed to create window surface (Linux)!");
	}

	// GLFW should create it now
	if (glfwCreateWindowSurface(vkInstance, window, nullptr, &vkSurface) != VK_SUCCESS) {
		Error("GLFW: Failed to create window surface!");
	}
#endif
}

/*
* ================================
*	Internal
* ================================
*/

/*
* Initilization functions
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
			// Get specs of the device
			VkPhysicalDeviceProperties vkProps;
			vkGetPhysicalDeviceProperties(dev, &vkProps);
			chosenGraphicsCard.name = std::string{vkProps.deviceName};
			debugPrint("Nexus::VulkanAPI::vulkanDevicePick", std::string{"Chose device " + std::string{vkProps.deviceName}}, LOG_INFO);
			vkPhysDevice = dev;
			break;
		}
	}
}


/*
*	Find Queue Families for a Physical Device
*/
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
//#ifndef __linux__
		vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, vkSurface, &presSupport);
//#endif
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


/*
*	Check if a device is suitable for our game
*/
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


/*
*	Check Extension support
*/
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

/*
*	Check swap chain support
*/
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
		
	}


	return details;
}

/*
*	Create a logical device
*/
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
	vkGetDeviceQueue(vkDevice, ind.graphicsFam.value(), 0, &vkGraphicsQueue);

}

/*
* Swap setting setup
*/
void Nexus::VulkanAPI::vulkanCreateSwapChain() {
	SwapChainSupportDetails swapChainS = getSwapChainSupport(vkPhysDevice);

	// Get settings
	VkSurfaceFormatKHR surfForm = chooseSwapSurfFormat(swapChainS.formats);
	VkPresentModeKHR presMode = chooseSwapPresMode(swapChainS.preModes);
	VkExtent2D extent = chooseSwapExt(swapChainS.caps);

	// Set images in swap chain
	uint32_t imageCount = swapChainS.caps.minImageCount + 1;
	// Make sure we dont exceed the max imageCount when we do the "+1"
	if (swapChainS.caps.maxImageCount > 0 && imageCount > swapChainS.caps.maxImageCount) {
		imageCount = swapChainS.caps.maxImageCount;
	}
	
	// Fill the swap chain structure
	VkSwapchainCreateInfoKHR crInfoSwap{};
	crInfoSwap.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	crInfoSwap.surface = vkSurface;
	crInfoSwap.minImageCount = imageCount;
	crInfoSwap.imageFormat = surfForm.format;
	crInfoSwap.imageColorSpace = surfForm.colorSpace;
	crInfoSwap.imageExtent = extent;
	crInfoSwap.imageArrayLayers = 1; // layers each image has
	/*
	*	If we doin post-processing, change this
	*/
	crInfoSwap.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // what tf we doin? we directly renedering em

	// How to handle swap chain images over multiple queue fams
	QueueFamilyIndicies inds = findQueueFams(vkPhysDevice);
	uint32_t queFamInd[] = {
		inds.graphicsFam.value(),
		inds.presentFam.value()
	};

	// Are they not the same?
	if (inds.graphicsFam != inds.presentFam) {
		// Then they must be in differnt queue fams
		crInfoSwap.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		crInfoSwap.queueFamilyIndexCount = 2;
		crInfoSwap.pQueueFamilyIndices = queFamInd;
	}
	else {
		// They're the same
		crInfoSwap.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	// We dont want any transformation
	crInfoSwap.preTransform = swapChainS.caps.currentTransform;

	// This is actually pretty f**king cool, we can blend with other windows
	// but alas, no need for that now
	crInfoSwap.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	crInfoSwap.presentMode = presMode;
	crInfoSwap.clipped = VK_TRUE;
	// We'll deal with resizing later
	crInfoSwap.oldSwapchain = VK_NULL_HANDLE;

	// Finally, create the swap chain
	if (vkCreateSwapchainKHR(vkDevice, &crInfoSwap, nullptr, &vkSwapChain) != VK_SUCCESS) {
		Error("Vulkan: Failed to create swap chain!");
	}

	// Prepare vector of images
	vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &imageCount, nullptr);
	vkSwapChainImgs.resize(imageCount);
	vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &imageCount, vkSwapChainImgs.data());

	// Set extent and format data
	vkSwapChainImgFmt = surfForm.format;
	vkSwapChainExt = extent;
}

VkSurfaceFormatKHR Nexus::VulkanAPI::chooseSwapSurfFormat(const std::vector<VkSurfaceFormatKHR>& avaForm) {
	// VkSurfaceFormatKHR contains format and colorSpace.
	// For format, we want VK_FORMAT_B8G8R8A8_SRGB (B, G, R, A 32 bit per pixel)
	// For color space, we want VK_COLOR_SPACE_SRGB_NONLINEAR_KHR (SRGB, standard color space for images)

	// Check if its avaliable
	for (const auto& avalFormat : avaForm) {
		if (avalFormat.format == VK_FORMAT_B8G8R8A8_SRGB && avalFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			debugPrint("Nexus::VulkanAPI::chooseSwapSurfFormat", "Got prefered surface format and color space!", LOG_INFO);
			// Got it
			return avalFormat;
		}
	}
	// gpu dosent have it. ugh. Fine, just use whats avlaible
	debugPrint("Nexus::VulkanAPI::chooseSwapSurfFormat", "Did not get prefered surface format and color space!", LOG_WARNING);
	return avaForm[0];
}

VkPresentModeKHR Nexus::VulkanAPI::chooseSwapPresMode(const std::vector<VkPresentModeKHR>& avaPres) {
	// 4 modes, but only VK_PRESENT_MODE_FIFO_KHR is guarenteed, so return that if all goes sad
	
	for (const auto& avalPres : avaPres) {
		if (avalPres == vkPreferedSwapPresentationMode) {
			debugPrint("Nexus::VulkanAPI::chooseSwapPresMode", "Got prefered presentation mode.", LOG_INFO);
			// Got it
			return avalPres;
		}
	}
	debugPrint("Nexus::VulkanAPI::chooseSwapPresMode", "Did not get prefered presentation mode! Defaulting to FIFO.", LOG_WARNING);
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Nexus::VulkanAPI::chooseSwapExt(const VkSurfaceCapabilitiesKHR& caps) {
	// The Swap extent is the resoultion 
	if (caps.currentExtent.width != 0xFFFFFFFF) {
		return caps.currentExtent;
	}
	else {
		debugPrint("Nexus::VulkanAPI::chooseSwapExt", "caps.currentExtent.width is equal to 0xFFFFFFFF, manually setting swap extent.", LOG_INFO);
		// We have to manually set it
		// Create temp extent
		VkExtent2D tmp = {
			(uint32_t)width,
			(uint32_t)height
		};

		// Clamp into the allowed values
		tmp.width = std::clamp(tmp.width, caps.minImageExtent.width, caps.maxImageExtent.width);
		tmp.height = std::clamp(tmp.height, caps.minImageExtent.height, caps.maxImageExtent.height);

		return tmp;
	}
}

void Nexus::VulkanAPI::vulkanCreateImageViews() {
	// Resize vector to fit images
	vkSwapChainImgViews.resize(vkSwapChainImgs.size());
	
	// Loop over each image and create an image view
	for (size_t i = 0; i < vkSwapChainImgs.size(); i++) {
		VkImageViewCreateInfo crInfo{};
		crInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		crInfo.image = vkSwapChainImgs[i];

		// Keep default colors
		crInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		crInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		crInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		crInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// Only color targets
		crInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		crInfo.subresourceRange.baseArrayLayer = 0;
		crInfo.subresourceRange.baseMipLevel = 0;
		crInfo.subresourceRange.levelCount = 1;
		crInfo.subresourceRange.layerCount = 1;

		// Create image view
		if (vkCreateImageView(vkDevice, &crInfo, nullptr, &vkSwapChainImgViews[i]) != VK_SUCCESS) {
			Error("Vulkan: Failed to create image views!");
		}
	}
}

void Nexus::VulkanAPI::vulkanCreateGraphicsPipeline(){

}

/*
* === END INIT FUNCS ===
*/


void Nexus::VulkanAPI::debugPrint(std::string caller, std::string text, int level) {
#if GRAPHICS_LOG == 1
	switch (level) {
	case LOG_INFO:
		PLOG_INFO << caller << " : " << text;
		break;
	case LOG_WARNING:
		PLOG_WARNING << caller << " : " << text;
		break;
	case LOG_ERROR:
		PLOG_ERROR << caller << " : " << text;
		break;
	}
#endif
}