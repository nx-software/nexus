/*
 *	Graphics rendering via the Vulkan Graphics API
 */

#include "vulkanApi.h"

#if VULKAN_ENABLED == 1

Nexus::VulkanAPI::VulkanAPI(GLFWwindow* window) {
	// Set user pointer
	glfwSetWindowUserPointer(window, this);
	// Set defaults
#ifndef __ANDROID__
#if defined(_WIN32) || defined(__linux__) || defined(mac)
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
	// Create descriptor set
	vulkanCreateDescriptorSetLayout();
	// We wait to create the pipeline until
	// a scene is initlized
	// Create frame buffers
	vulkanCreateFramebuffers();
	// Create command pool
	vulkanCreateCommandPool();
	// Create vertex buffer
	vulkanCreateVertexBuffer();
	// Create index
	vulkanCreateIndexBuffer();
	// Create uniform buffers
	vulkanCreateUniformBuffers();
	// Create descriptor pool
	vulkanCreateDescriptorPool();
	// Create descriptor sets
	vulkanCreateDescriptorSets();
	// Create sync objects
	vulkanCreateSyncObjects();
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
	for (size_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
		// Destroy semaphores
		vkDestroySemaphore(vkDevice, vkImageAvaSem[i], nullptr);
		vkDestroySemaphore(vkDevice, vkRenderFinishedSem[i], nullptr);
		// Destroy fence
		vkDestroyFence(vkDevice, vkInFlightFen[i], nullptr);
	}

	for (size_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
		vkDestroyBuffer(vkDevice, vkUniBuf[i], nullptr);
		vkFreeMemory(vkDevice, vkUniBufMem[i], nullptr);
	}

	vkDestroyDescriptorPool(vkDevice, vkDescPool, nullptr);
	vkDestroyDescriptorSetLayout(vkDevice, vkDescSetLayout, nullptr);

	// Destroy command pool
	vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);

	// Destroy pipeline layout
	vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, nullptr);
	// Destroy Render pass
	vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);

	// Destroy swap chain
	vulkanCleanSwapChain();

	vkDestroyBuffer(vkDevice, vkIndexStagingBuf, nullptr);
	vkFreeMemory(vkDevice, vkIndexStageBufMem, nullptr);

	vkDestroyBuffer(vkDevice, vkIndexBuffer, nullptr);
	vkFreeMemory(vkDevice, vkIndexBufferMem, nullptr);

	vkDestroyBuffer(vkDevice, vkVertexStagingBuf, nullptr);
	vkFreeMemory(vkDevice, vkVertexBufferMem, nullptr);

	vkDestroyBuffer(vkDevice, vkVertexBuffer, nullptr);
	vkFreeMemory(vkDevice, vkVertexBufferMem, nullptr);


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

GLFWframebuffersizefun Nexus::VulkanAPI::SetupWindowResize() {
	return Nexus::frameBufResizeCallback;
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
	vkRasterCrInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

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
	vkColorBlendCrInfo.attachmentCount = 1;
	vkColorBlendCrInfo.pAttachments = &vkColorBlendState;

	// Disable multisampling for now
	vkMultisampleCrInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	vkMultisampleCrInfo.sampleShadingEnable = VK_FALSE;
	vkMultisampleCrInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Lets create it
	vkPipeLineLayoutCrInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	vkPipeLineLayoutCrInfo.setLayoutCount = 1;
	vkPipeLineLayoutCrInfo.pSetLayouts = &vkDescSetLayout;
	// Some other optional stuff, but we're ok for now

	if(vkCreatePipelineLayout(vkDevice, &vkPipeLineLayoutCrInfo, nullptr, &vkPipelineLayout) != VK_SUCCESS){
		Error("Vulkan: Failed to create pipeline layout!");
	}

	// Create things for each
	for(auto& gm : scene->getObjects()){
		auto vert = gm->getVertShader()->readShader();
		auto frag = gm->getFragShader()->readShader();

		// Create shader modules
		VkShaderModule vertShaderMod = createShaderModule(vert);
		VkShaderModule fragShaderMod = createShaderModule(frag);

		// Create pipelines
		VkPipelineShaderStageCreateInfo vertPipe{}, fragPipe{};

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

		auto bindDesc = Nexus::Vertex::getBindDesc();
		auto attrDesc = Nexus::Vertex::getAttrDesc();
	
		vkShader.vertCrInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vkShader.vertCrInfo.vertexBindingDescriptionCount = 1;
		vkShader.vertCrInfo.pVertexBindingDescriptions = &bindDesc;
		vkShader.vertCrInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrDesc.size());
		vkShader.vertCrInfo.pVertexAttributeDescriptions = attrDesc.data();

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
		pipelineCrInfo.pTessellationState = nullptr;
		pipelineCrInfo.pColorBlendState = &vkColorBlendCrInfo;
		pipelineCrInfo.pDynamicState = &dynamicState;
		pipelineCrInfo.layout = vkPipelineLayout;
		pipelineCrInfo.renderPass = vkRenderPass;
		pipelineCrInfo.subpass = 0;
		// Other renderpasses can be used but we dont need em rn
		pipelineCrInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCrInfo.basePipelineIndex = -1;
		pipelineCrInfo.pNext = nullptr;

		vkShader.vertexBufferOffset = currentVertexOffset;
		vkShader.indexBufferOffset = currentIndexOffset;

		currentVertexOffset += sizeof(Nexus::Vertex) * gm->mesh->getVertices().size();
		currentIndexOffset += sizeof(uint16_t) * gm->mesh->getIndicies().size();
		
		// Create the pipline
		if(vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineCrInfo, nullptr, &(vkShader.grPipeline)) != VK_SUCCESS){
			Error("Vulkan: Failed to create graphics pipline!");
		}

		gm->gShader = new VulkanShader(vkShader);


		debugPrint("Nexus::VulkanAPI::InitShaders", std::string{"Loaded 1 object (" + gm->getName() + ")"}, 0);
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

	// Get presentation queue
	vkGetDeviceQueue(vkDevice, ind.presentFam.value(), 0, &vkPresentQueue);

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
		crInfo.format = vkSwapChainImgFmt;
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
		Error("Vulkan: Failed to create shader module!\n");
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

	// We need to time subpasses correctly, as there are two
	// and the first one is not timed right. we could fix it with
	// VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT on vkImageAvaSem, 
	// but since im gonna have to implement subpass depends
	// anyway might as well try to learn em right now
	VkSubpassDependency dep{};
	dep.srcSubpass = VK_SUBPASS_EXTERNAL;
	dep.dstSubpass = 0; // aw
	dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep.srcAccessMask = 0;
	dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // wait on the color to happen
	dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttach;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDesc;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dep;

	if(vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &vkRenderPass) != VK_SUCCESS){
		Error("Vulkan: Failed to create render pass!");
	}

}

void Nexus::VulkanAPI::vulkanCreateDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding camDataLayoutBinding{};
	camDataLayoutBinding.binding = 0;
	camDataLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	camDataLayoutBinding.descriptorCount = 1;
	camDataLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	camDataLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo layoutInf{};
	layoutInf.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInf.bindingCount = 1;
	layoutInf.pBindings = &camDataLayoutBinding;

	if (vkCreateDescriptorSetLayout(vkDevice, &layoutInf, nullptr, &vkDescSetLayout) != VK_SUCCESS) {
		Error("Vulkan: Failed to create descriptor set layout!");
	}
}

void Nexus::VulkanAPI::vulkanCreateGraphicsPipeline(){
	
}

void Nexus::VulkanAPI::vulkanCreateFramebuffers(){
	vkSwapChainFrameBuf.resize(vkSwapChainImgViews.size());
	// Iterate over and create buffers	
	for(size_t i = 0; i < vkSwapChainImgViews.size(); i++){
		VkImageView attachments[] = {
			vkSwapChainImgViews[i]
		};

		VkFramebufferCreateInfo frameBufCrInfo{};
		frameBufCrInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufCrInfo.renderPass = vkRenderPass;
		frameBufCrInfo.attachmentCount = 1;
		frameBufCrInfo.pAttachments = attachments;
		frameBufCrInfo.width = vkSwapChainExt.width;
		frameBufCrInfo.height = vkSwapChainExt.height;
		frameBufCrInfo.layers = 1;

		// create it!
		if(vkCreateFramebuffer(vkDevice, &frameBufCrInfo, nullptr, &vkSwapChainFrameBuf[i]) != VK_SUCCESS){
			Error(std::string{"Vulkan: Failed to create framebuffer number" + std::to_string(i)});
		}
	}
}

void Nexus::VulkanAPI::vulkanCreateCommandPool(){
	// Command pool needs queue familes
	QueueFamilyIndicies qFam = findQueueFams(vkPhysDevice);

	VkCommandPoolCreateInfo commandPoolCrInfo{};
	commandPoolCrInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	// allow command buffer to be rerecorded individually
	commandPoolCrInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCrInfo.queueFamilyIndex = qFam.graphicsFam.value();

	if(vkCreateCommandPool(vkDevice, &commandPoolCrInfo, nullptr, &vkCommandPool) != VK_SUCCESS){
		Error("Vulkan: Failed to create command pool!");
	}

	debugPrint("Nexus::VulkanAPI::vulkanCreateCommandPool", "Created Command Pool", 0);

	// Create command buffer while we're at it
	vkCommandBuffer.resize(MAX_FRAME_IN_FLIGHT);

	VkCommandBufferAllocateInfo commandBufAlInfo{};
	commandBufAlInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufAlInfo.commandPool = vkCommandPool;
	commandBufAlInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // can be submitted to a queue
	commandBufAlInfo.commandBufferCount = (uint32_t) MAX_FRAME_IN_FLIGHT;

	if(vkAllocateCommandBuffers(vkDevice, &commandBufAlInfo, vkCommandBuffer.data()) != VK_SUCCESS){
		Error("Vulkan: Failed to allocate command buffer!");
	}

	debugPrint("Nexus::VulkanAPI::vulkanCreateCommandPool", "Despite the function name, allocation of command buffer was successful.", 0);
}

void Nexus::VulkanAPI::vulkanCreateVertexBuffer() {
	// Allocate a bunch of memory so we don't gotta worry about it
	VkDeviceSize bufSize = VERTEX_BUFFER_SIZE;
	
	// Lets make a staging buffer
	// We can use this to copy our vertex data
	vulkanCreateBuffer(bufSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vkVertexStagingBuf, vkVertexStageBufMem);
	
	// Create Vertex buffer
	vulkanCreateBuffer(bufSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkVertexBuffer, vkVertexBufferMem);
}

void Nexus::VulkanAPI::vulkanCreateIndexBuffer() {
	VkDeviceSize bufSize = INDEX_BUFFER_SIZE;
	
	// We can use this to copy our vertex data
	vulkanCreateBuffer(bufSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vkIndexStagingBuf, vkIndexStageBufMem);

	// Create Vertex buffer
	vulkanCreateBuffer(bufSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkIndexBuffer, vkIndexBufferMem);
}

void Nexus::VulkanAPI::vulkanCreateUniformBuffers() {
	VkDeviceSize bufSize = sizeof(Nexus::CameraData);

	vkUniBuf.resize(MAX_FRAME_IN_FLIGHT);
	vkUniBufMap.resize(MAX_FRAME_IN_FLIGHT);
	vkUniBufMem.resize(MAX_FRAME_IN_FLIGHT);

	for (int i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
		vulkanCreateBuffer(bufSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vkUniBuf[i], vkUniBufMem[i]);
		vkMapMemory(vkDevice, vkUniBufMem[i], 0, bufSize, 0, &vkUniBufMap[i]);
	}

}

void Nexus::VulkanAPI::vulkanCreateDescriptorPool() {
	VkDescriptorPoolSize poolSz{};
	poolSz.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSz.descriptorCount = static_cast<uint32_t>(MAX_FRAME_IN_FLIGHT);
	
	VkDescriptorPoolCreateInfo poolCrInf{};
	poolCrInf.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCrInf.poolSizeCount = 1;
	poolCrInf.pPoolSizes = &poolSz;
	poolCrInf.maxSets = static_cast<uint32_t>(MAX_FRAME_IN_FLIGHT);

	if (vkCreateDescriptorPool(vkDevice, &poolCrInf, nullptr, &vkDescPool) != VK_SUCCESS) {
		Error("Vulkan: Failed to create descriptor pool!");
	}
}

void Nexus::VulkanAPI::vulkanCreateDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAME_IN_FLIGHT, vkDescSetLayout);
	VkDescriptorSetAllocateInfo allocInf{};
	allocInf.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInf.descriptorPool = vkDescPool;
	allocInf.descriptorSetCount = static_cast<uint32_t>(MAX_FRAME_IN_FLIGHT);
	allocInf.pSetLayouts = layouts.data();

	vkDescSets.resize(MAX_FRAME_IN_FLIGHT);
	if (vkAllocateDescriptorSets(vkDevice, &allocInf, vkDescSets.data()) != VK_SUCCESS) {
		Error("Vulkan: Failed to allocate descriptor sets!");
	}

	// Fill out every descriptor
	for (size_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufInf{};
		bufInf.buffer = vkUniBuf[i];
		bufInf.offset = 0;
		bufInf.range = sizeof(Nexus::CameraData);
		
		VkWriteDescriptorSet descWr{};
		descWr.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descWr.dstSet = vkDescSets[i];
		descWr.dstBinding = 0;
		descWr.dstArrayElement = 0;
		descWr.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descWr.descriptorCount = 1;
		descWr.pBufferInfo = &bufInf;
		descWr.pImageInfo = nullptr;
		descWr.pTexelBufferView = nullptr;
		
		vkUpdateDescriptorSets(vkDevice, 1, &descWr, 0, nullptr);
	}
}

void Nexus::VulkanAPI::vulkanCreateSyncObjects() {
	// prepare for flight frames
	vkImageAvaSem.resize(MAX_FRAME_IN_FLIGHT);
	vkRenderFinishedSem.resize(MAX_FRAME_IN_FLIGHT);
	vkInFlightFen.resize(MAX_FRAME_IN_FLIGHT);

	VkSemaphoreCreateInfo smInf{};
	smInf.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInf{};
	fenceInf.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	// We add this so its already signaled on init,
	// so that on the first frame we dont wait for it
	fenceInf.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
	for (size_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
		// Create each semaphore
		if (vkCreateSemaphore(vkDevice, &smInf, nullptr, &vkImageAvaSem[i]) != VK_SUCCESS) {
			Error("Vulkan: Failed to create 'image avaliable' semaphore!");
		}
		if (vkCreateSemaphore(vkDevice, &smInf, nullptr, &vkRenderFinishedSem[i]) != VK_SUCCESS) {
			Error("Vulkan: Failed to create 'render finished' semaphore!");
		}
		if (vkCreateFence(vkDevice, &fenceInf, nullptr, &vkInFlightFen[i]) != VK_SUCCESS) {
			Error("Vulkan: Failed to create 'in flight' fence!");
		}
	}
}

/*
* === END INIT FUNCS ===
*/

void Nexus::VulkanAPI::vulkanCreateBuffer(VkDeviceSize devSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkBuffer& buffer, VkDeviceMemory& bufMem) {
	VkBufferCreateInfo bufCrInfo{};
	bufCrInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufCrInfo.size = devSize;
	bufCrInfo.usage = usage;
	bufCrInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // only used from the graphics queue

	if (vkCreateBuffer(vkDevice, &bufCrInfo, nullptr, &buffer) != VK_SUCCESS) {
		Error("Vulkan: Failed to create buffer!");
	}

	// Memory requirments
	VkMemoryRequirements vkMemReq;
	// Allocate memory
	vkGetBufferMemoryRequirements(vkDevice, buffer, &vkMemReq);
	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.allocationSize = vkMemReq.size;
	memAllocInfo.memoryTypeIndex = findMemType(vkMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(vkDevice, &memAllocInfo, nullptr, &bufMem) != VK_SUCCESS) {
		Error("Vulkan: Failed to allocate buffer memory!");
	}

	// Bind memory
	vkBindBufferMemory(vkDevice, buffer, bufMem, 0);
}

void Nexus::VulkanAPI::vulkanCopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
	VkCommandBufferAllocateInfo cmdBufAllocInfo{};
	cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufAllocInfo.commandPool = vkCommandPool;
	cmdBufAllocInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuf;
	if (vkAllocateCommandBuffers(vkDevice, &cmdBufAllocInfo, &cmdBuf) != VK_SUCCESS) {
		Error("Vulkan: Failed to create command buffer for copying!");
	}

	VkCommandBufferBeginInfo beginInf;
	beginInf.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInf.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInf.pNext = nullptr;

	vkBeginCommandBuffer(cmdBuf, &beginInf);

	VkBufferCopy copyReg;
	copyReg.srcOffset = srcOffset;
	copyReg.dstOffset = dstOffset;
	copyReg.size = size;
	vkCmdCopyBuffer(cmdBuf, src, dst, 1, &copyReg);
	
	vkEndCommandBuffer(cmdBuf);

	VkSubmitInfo subInfo{};
	subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	subInfo.commandBufferCount = 1;
	subInfo.pCommandBuffers = &cmdBuf;

	vkQueueSubmit(vkGraphicsQueue, 1, &subInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(vkGraphicsQueue);
	vkFreeCommandBuffers(vkDevice, vkCommandPool, 1, &cmdBuf);
}

void Nexus::VulkanAPI::vulkanUpdateMeshBuffers(Nexus::GameObject* obj) {
	// Lets copy our vertex data
	// We gotta remember our offset !
	VulkanShader* shader = (VulkanShader*)obj->gShader;	
	// Map the memory into CPU accessible memory
	void* data;
	vkMapMemory(vkDevice, vkVertexStageBufMem, shader->vertexBufferOffset, sizeof(Nexus::Vertex) * obj->mesh->getVertices().size(), 0, &data);
	memcpy(data, obj->mesh->getVertices().data(), sizeof(Nexus::Vertex) * obj->mesh->getVertices().size());
	vkUnmapMemory(vkDevice, vkVertexStageBufMem);

	// Copy the buffer
	vulkanCopyBuffer(vkVertexStagingBuf, vkVertexBuffer, sizeof(Nexus::Vertex) * obj->mesh->getVertices().size(), shader->vertexBufferOffset, shader->vertexBufferOffset);

	// Ok Index buffer next
	void* idata;
	vkMapMemory(vkDevice, vkIndexStageBufMem, shader->indexBufferOffset, sizeof(uint16_t) * obj->mesh->getIndicies().size(), 0, &idata);
	memcpy(idata, obj->mesh->getIndicies().data(), sizeof(uint16_t) * obj->mesh->getIndicies().size());
	vkUnmapMemory(vkDevice, vkIndexStageBufMem);

	// Copy the buffer
	vulkanCopyBuffer(vkIndexStagingBuf, vkIndexBuffer, sizeof(uint16_t) * obj->mesh->getIndicies().size(), shader->indexBufferOffset, shader->indexBufferOffset);
}

void Nexus::VulkanAPI::vulkanRecordCommandBuffer(uint32_t idx, std::vector<GameObject*> objs){
	VkCommandBufferBeginInfo bufferBeginInf{};
	bufferBeginInf.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	// Has some extra stuff but we dont need them rn
	bufferBeginInf.flags = 0;
	bufferBeginInf.pInheritanceInfo = nullptr;

	if(vkBeginCommandBuffer(vkCommandBuffer[vkCurFrame], &bufferBeginInf) != VK_SUCCESS) {
		Error("Vulkan: Failed to begin recording the command buffer!");
	}

	VkRenderPassBeginInfo renderPassBeginInf{};
	renderPassBeginInf.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInf.renderPass = vkRenderPass;
	renderPassBeginInf.framebuffer = vkSwapChainFrameBuf[idx];
	renderPassBeginInf.renderArea.offset = {0, 0};
	renderPassBeginInf.renderArea.extent = vkSwapChainExt;
	
	// Define color for CLEAR
	VkClearValue clearCol = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	renderPassBeginInf.clearValueCount = 1;
	renderPassBeginInf.pClearValues = &clearCol;

	// Begin our render pass
	//                                                         embedded into primary buffer
	vkCmdBeginRenderPass(vkCommandBuffer[vkCurFrame], &renderPassBeginInf, VK_SUBPASS_CONTENTS_INLINE);
	for (auto& gm : objs) {
		VulkanShader* shader = (VulkanShader*)gm->gShader;
		VkPipeline grPipeline = shader->grPipeline;
		vkCmdBindPipeline(vkCommandBuffer[vkCurFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, grPipeline);

		vkCmdSetViewport(vkCommandBuffer[vkCurFrame], 0, 1, &vkViewport);
		vkCmdSetScissor(vkCommandBuffer[vkCurFrame], 0, 1, &vkScissor);

		VkBuffer vertBufs[] = { vkVertexBuffer };
		VkDeviceSize offsets[] = { shader->vertexBufferOffset };
		vkCmdBindVertexBuffers(vkCommandBuffer[vkCurFrame], 0, 1, vertBufs, offsets);
		vkCmdBindIndexBuffer(vkCommandBuffer[vkCurFrame], vkIndexBuffer, shader->indexBufferOffset, VK_INDEX_TYPE_UINT16);

		// Bind desc sets
		vkCmdBindDescriptorSets(vkCommandBuffer[vkCurFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, 1, &vkDescSets[vkCurFrame], 0, nullptr);

		//               verts, no instance rendering
		vkCmdDrawIndexed(vkCommandBuffer[vkCurFrame], static_cast<uint32_t>(gm->mesh->getIndicies().size()), 1, 0, 0, 0);
	}

	// End
	vkCmdEndRenderPass(vkCommandBuffer[vkCurFrame]);

	if(vkEndCommandBuffer(vkCommandBuffer[vkCurFrame]) != VK_SUCCESS) {
		Error("Vulkan: Failed to end command buffer!");
	}
}

void Nexus::VulkanAPI::vulkanCleanSwapChain() {
	for (size_t i = 0; i < vkSwapChainFrameBuf.size(); i++) {
		vkDestroyFramebuffer(vkDevice, vkSwapChainFrameBuf[i], nullptr);
	}
	for (size_t i = 0; i < vkSwapChainImgViews.size(); i++) {
		vkDestroyImageView(vkDevice, vkSwapChainImgViews[i], nullptr);
	}

	vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
}

void Nexus::VulkanAPI::vulkanRecreateSwapChain() {
	vkDeviceWaitIdle(vkDevice);

	vulkanCleanSwapChain();

	vulkanCreateSwapChain();
	vulkanCreateImageViews();
	vulkanCreateFramebuffers();

	frameBufResize = false;
}

uint32_t Nexus::VulkanAPI::findMemType(uint32_t filter, VkMemoryPropertyFlags props) {
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(vkPhysDevice, &memProps);

	// VkPhysicalDeviceMemoryProperties has two arrays, memTypes and memHeaps/
	// memHeaps are things like dedicated VRAM and RAM swap
	// rn lowk we only need the type of memory, not what heap it originates from

	for (uint32_t i = 0; i < memProps.memoryTypeCount; i++){
		if (filter & (1 << i) && (memProps.memoryTypes[i].propertyFlags & props)) { // simpily look and see if the corresponding bit is enabled and if we can write to it
			return i;
		}
	}
}

void Nexus::VulkanAPI::DrawFrame(Scene* scene) {
	// From a high level overview, we wanna to:
	// 1. wait for previous frame to draw
	vkWaitForFences(vkDevice, 1, &vkInFlightFen[vkCurFrame], VK_TRUE, UINT64_MAX);
	// 1.5 update our camera
	cam->camData.proj = glm::perspective(glm::radians((float)cam->pAngle), vkSwapChainExt.width / (float)vkSwapChainExt.height, 0.1f, 10.0f);
	cam->camData.proj[1][1] *= -1;
	memcpy(vkUniBufMap[vkCurFrame], &(cam->camData), sizeof(Nexus::CameraData));
	// 2. aquire an image from the swap chain
	uint32_t imgIdx;
	// Grab result to see if we gotta change our frame
	VkResult res = vkAcquireNextImageKHR(vkDevice, vkSwapChain, UINT64_MAX, vkImageAvaSem[vkCurFrame], VK_NULL_HANDLE, &imgIdx);
	if (res == VK_ERROR_OUT_OF_DATE_KHR || frameBufResize) {
		vulkanRecreateSwapChain();
		return;
	}
	else if ((!VK_SUCCESS) && (!VK_SUBOPTIMAL_KHR)) {
		Error("Vulkan: Acquire swap chain image failed!");
	}
	vkResetFences(vkDevice, 1, &vkInFlightFen[vkCurFrame]);
	// 3. record a command buffer
	vkResetCommandBuffer(vkCommandBuffer[vkCurFrame], 0);
	for (auto& gm : scene->getObjects()) {		
		// Do we need to update
		if (gm->mesh->modified) {
			vulkanUpdateMeshBuffers(gm);
		}	
	}
	// Get shader
	vulkanRecordCommandBuffer(imgIdx, scene->getObjects());
	// 4. submit that command buffer
	VkSubmitInfo smInfo{};
	smInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSems[] = { vkImageAvaSem[vkCurFrame] };
	VkPipelineStageFlags  waitSta[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	smInfo.waitSemaphoreCount = 1;
	smInfo.pWaitSemaphores = waitSems;
	smInfo.pWaitDstStageMask = waitSta;
	// Lets submit the only command buffer we have (as of right now)
	smInfo.commandBufferCount = 1;
	smInfo.pCommandBuffers = &vkCommandBuffer[vkCurFrame];

	VkSemaphore signalSem[] = { vkRenderFinishedSem[vkCurFrame] };
	smInfo.signalSemaphoreCount = 1;
	smInfo.pSignalSemaphores = signalSem;

	// Submit
	if (vkQueueSubmit(vkGraphicsQueue, 1, &smInfo, vkInFlightFen[vkCurFrame]) != VK_SUCCESS) {
		Error("Vulkan: Failed to submit draw command buffer!");
	}	

	// 5. present it
	VkPresentInfoKHR presInf{};
	presInf.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presInf.waitSemaphoreCount = 1;
	presInf.pWaitSemaphores = signalSem;
	
	VkSwapchainKHR swapChains[] = { vkSwapChain };
	presInf.swapchainCount = 1;
	presInf.pSwapchains = swapChains;
	presInf.pImageIndices = &imgIdx;

	// since we only usin one swap chain
	// dont waste time checking if EVERY (1)
	// swap chain pres was succesful
	presInf.pResults = nullptr;

	// finally. . .
	res = vkQueuePresentKHR(vkPresentQueue, &presInf);
	if (res == VK_ERROR_OUT_OF_DATE_KHR || frameBufResize) {
		vulkanRecreateSwapChain();
		return;
	}
	else if ((!VK_SUCCESS) && (!VK_SUBOPTIMAL_KHR)) {
		Error("Vulkan: Present swap chain image failed!");
	}

	// wait for device to be done
	vkDeviceWaitIdle(vkDevice);

	// change frame
	vkCurFrame = (vkCurFrame + 1) % MAX_FRAME_IN_FLIGHT;

	
}


void Nexus::VulkanAPI::debugPrint(std::string caller, std::string text, int level) {
	printf("%s - %s\n",caller.c_str(), text.c_str());
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
/*
	=== Vulkan Shader Holder ===
*/

// Nexus::VulkanShader::VulkanShader(){

// }


// Static function for window resize callbacks
static void Nexus::frameBufResizeCallback(GLFWwindow* win, int w, int h) {
	auto vApi = reinterpret_cast<VulkanAPI*>(glfwGetWindowUserPointer(win));
	vApi->setFBResize(true);
}



#endif // VULKAN_ENABLED == 1