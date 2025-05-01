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
	// Create render pass
	vulkanCreateRenderPass();
	// We wait to create the pipeline until
	// a scene is initlized
}

void Nexus::VulkanAPI::CleanScene(Scene* scene){
	for(auto& gm : scene->getObjects()){
		VulkanShader* shader = (VulkanShader*)(gm->gShader);
		vkDestroyPipeline(vkDevice, shader->grPipeline, nullptr);
		vkDestroyShaderModule(vkDevice, shader->vert, nullptr);
		vkDestroyShaderModule(vkDevice, shader->frag, nullptr);
	}
}


void Nexus::VulkanAPI::Clean() {
	// Destroy pipeline layout
	vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, nullptr);
	// Destroy Render pass
	vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
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

void Nexus::VulkanAPI::InitShaders(Scene* scene){
	// Create pipeline
	
	// Start with dynamic pipeline stuff
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();
	// Woah, now we can modify this stuff at runtime without
	// having to recreate the entire pipeline

	// Create viewport stuff
	vkViewport.x = 0.0f;
	vkViewport.y = 0.0f;
	// Since swap chain sizes may differ from window size,
	// so we'll use the swap chain sizes instead
	vkViewport.width = (float) vkSwapChainExt.width;
	vkViewport.height = (float) vkSwapChainExt.height;
	// Not doing anything interesting rn so 
	// we might as well keep it at normal range
	vkViewport.minDepth = 0.0f;
	vkViewport.maxDepth = 1.0f;

	// Scissor viewports define where pixels
	// where actually be stored, for now lets
	// just keep it at the whole window
	vkScissor.offset = {0, 0};
	vkScissor.extent = vkSwapChainExt;

	// Creation info for viewport stuff
	vkPipeLineViewPortCrInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vkPipeLineViewPortCrInfo.viewportCount = 1;
	vkPipeLineViewPortCrInfo.pViewports = &vkViewport;
	vkPipeLineViewPortCrInfo.scissorCount = 1;
	vkPipeLineViewPortCrInfo.pScissors = &vkScissor;

	// Creation stuff for raster
	vkRasterCrInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	vkRasterCrInfo.depthClampEnable = VK_FALSE;
	// How we want to draw things
	vkRasterCrInfo.polygonMode = VK_POLYGON_MODE_FILL;
	// How thick the lines shoulc be
	vkRasterCrInfo.lineWidth = 1.0f;
	// Culling mode
	vkRasterCrInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	vkRasterCrInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

	// Color blending stuff
	// Defines how we blend the fragment shader from our current
	// frame to the next
	vkColorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
	vkColorBlendState.blendEnable = VK_FALSE;
	// Theres some other features but we dont need them rn

	// Color blend creation
	vkColorBlendCrInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	vkColorBlendCrInfo.logicOpEnable = VK_FALSE;
	vkColorBlendCrInfo.logicOp = VK_LOGIC_OP_COPY;
	vkColorBlendCrInfo.pAttachments = &vkColorBlendState;
	vkColorBlendCrInfo.attachmentCount = 1;

	// Disable multisampling for now
	vkMultisampleCrInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	vkMultisampleCrInfo.sampleShadingEnable = VK_FALSE;
	vkMultisampleCrInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Lets create it
	vkPipeLineLayoutCrInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	// Some other optional stuff, but we're ok for now

	if(vkCreatePipelineLayout(vkDevice, &vkPipeLineLayoutCrInfo, nullptr, &vkPipelineLayout) != VK_SUCCESS){
		Error("Vulkan: Failed to create pipeline layout!");
	}

	// Create things for each
	for(auto& gm : scene->getObjects()){
		auto vert = gm->getVertShader().readShader();
		auto frag = gm->getFragShader().readShader();

		// Create shader modules
		VkShaderModule vertShaderMod = createShaderModule(vert);
		VkShaderModule fragShaderMod = createShaderModule(frag);

		// Create pipelines
		VkPipelineShaderStageCreateInfo vertPipe, fragPipe;

		// First vertex 
		vertPipe.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertPipe.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertPipe.pName = "main";

		// Then Fragment
		fragPipe.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragPipe.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragPipe.pName = "main";

		// 
		VulkanShader vkShader;
		vkShader.vert = vertShaderMod;
		vkShader.frag = fragShaderMod;

		vertPipe.module = vkShader.vert;
		fragPipe.module = vkShader.frag;

		vkShader.vertPipelineInfo = vertPipe;
		vkShader.fragPipelineInfo = fragPipe;
		vkShader.shaderStages[0] = vertPipe;
		vkShader.shaderStages[1] = fragPipe;

		// TODO: RIGHT NOW THIS JUST SAYS THE DATA IS IN THE SHADER
		// NEED TO MAKE IT READ FROM MESHES LATER ON
		vkShader.vertCrInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vkShader.vertCrInfo.vertexBindingDescriptionCount = 0;
		vkShader.vertCrInfo.pVertexBindingDescriptions = nullptr;
		vkShader.vertCrInfo.vertexAttributeDescriptionCount = 0;
		vkShader.vertCrInfo.pVertexBindingDescriptions = nullptr;

		// Input assembly
		vkShader.inputAsmCrInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		vkShader.inputAsmCrInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		vkShader.inputAsmCrInfo.primitiveRestartEnable = VK_FALSE;

		VkGraphicsPipelineCreateInfo pipelineCrInfo{};
		pipelineCrInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCrInfo.stageCount = 2;
		pipelineCrInfo.pStages = vkShader.shaderStages;
		pipelineCrInfo.pVertexInputState = &(vkShader.vertCrInfo);
		pipelineCrInfo.pInputAssemblyState = &(vkShader.inputAsmCrInfo);
		pipelineCrInfo.pViewportState = &vkPipeLineViewPortCrInfo;
		pipelineCrInfo.pRasterizationState = &vkRasterCrInfo;
		pipelineCrInfo.pMultisampleState = &vkMultisampleCrInfo;
		pipelineCrInfo.pDepthStencilState = nullptr;
		pipelineCrInfo.pColorBlendState = &vkColorBlendCrInfo;
		pipelineCrInfo.pDynamicState = &dynamicState;
		pipelineCrInfo.layout = vkPipelineLayout;
		pipelineCrInfo.renderPass = vkRenderPass;
		pipelineCrInfo.subpass = 0;
		// Other renderpasses can be used but we dont need em rn
		pipelineCrInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCrInfo.basePipelineIndex = -1;
		
		// Create the pipline
		if(vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineCrInfo, nullptr, &(vkShader.grPipeline)) != VK_SUCCESS){
			Error("Vulkan: Failed to create graphics pipline!");
		}

		gm->gShader = &vkShader;

		debugPrint("Nexus::VulkanAPI::InitShaders", std::string{"Loaded 1 object"}, 0);
	}


	// Create graphics pipeline
	vulkanCreateGraphicsPipeline();
}

/*
* ================================
*	Internal
* ================================
*/

bool Nexus::VulkanAPI::checkValidLayer(){
	uint32_t lCount;
	vkEnumerateInstanceLayerProperties(&lCount, nullptr);

	std::vector<VkLayerProperties> avaLayers(lCount);
	vkEnumerateInstanceLayerProperties(&lCount, avaLayers.data());

	for(const char* lName : validLayers){
		bool layerFound = false;
		for(const auto& layerProps : avaLayers){
			if(strcmp(lName, layerProps.layerName) == 0){
				layerFound = true;
				break;
			}
		}

		if(!layerFound){
			return false;
		}
	}

	return true;
}

/*
* Initilization functions
*/
void Nexus::VulkanAPI::vulkanCreateInstance() {
	// check if validation layers
	if(validLayer && !checkValidLayer()){
		Error("Vulkan: Validation layers requested but not supported");
	}else if(validLayer){
		debugPrint("Nexus::VulkanAPI::vulkanCreateInstance", "Validation layers supported.", 0);
	}
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
	if(validLayer){
		crInfo.enabledLayerCount = static_cast<uint32_t>(validLayers.size());
		crInfo.ppEnabledLayerNames = validLayers.data();
	}else{
		crInfo.enabledLayerCount = 0;
	}

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

VkShaderModule Nexus::VulkanAPI::createShaderModule(const std::vector<char>& code){
	VkShaderModuleCreateInfo shaderModCrInfo{};
	shaderModCrInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModCrInfo.codeSize = code.size();
	shaderModCrInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderMod;
	if(vkCreateShaderModule(vkDevice, &shaderModCrInfo, nullptr, &shaderMod) != VK_SUCCESS){
		Error("Failed to create shader module!\n");
	}

	return shaderMod;
}

void Nexus::VulkanAPI::vulkanCreateRenderPass(){
	// Color attachment
	VkAttachmentDescription colorAttach{};
	colorAttach.format = vkSwapChainImgFmt;
	// since we're not sampling, we just
	// stick to 1 sample
	colorAttach.samples = VK_SAMPLE_COUNT_1_BIT;
	// what we do before and after rendering
	colorAttach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear values before rendering
	colorAttach.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // keep them afterwards
	// we dont really care about the stencil buffer
	colorAttach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// framebuffers are represented with VkImage, so
	// we can choose what we do with the layout in 
	// memory
	colorAttach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; 
	colorAttach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // swap chain images format

	// VkAttachmentRefrence
	VkAttachmentReference colorAttachRef{};
	colorAttachRef.attachment = 0; // we have 1, colorAttach
	colorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // best preformance

	// Our subpass
	VkSubpassDescription subpassDesc{};
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.colorAttachmentCount = 1;
	subpassDesc.pColorAttachments = &colorAttachRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttach;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDesc;

	if(vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &vkRenderPass) != VK_SUCCESS){
		Error("Vulkan: Failed to create render pass!");
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

/*
	=== Vulkan Shader Holder ===
*/

// Nexus::VulkanShader::VulkanShader(){

// }

}