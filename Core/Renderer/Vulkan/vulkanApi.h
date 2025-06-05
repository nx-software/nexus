/*
*	Class for Vulkan rendering
*/
#pragma once

#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <cstdint>
#include <limits>
#include <algorithm>

#if VULKAN == 1
#ifdef __linux__
#include <xcb/xcb.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_xcb.h>
#include <X11/Xlib-xcb.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif
#endif

#include "../renderApi.h"
#include "../../../Engine/config.h"
//#include "../shader.h"
#include "../../../Engine/gameObject.h"

// Logging

#include <plog/Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Appenders/ConsoleAppender.h>

#define LOG_INFO 0
#define LOG_WARNING 1
#define LOG_ERROR 2

#define DEBUG 1

#define VERTEX_BUFFER_SIZE (size_t)40000
#define INDEX_BUFFER_SIZE VERTEX_BUFFER_SIZE/2

namespace Nexus {
#if VULKAN == 1
	// Make this configurable later
	const int MAX_FRAME_IN_FLIGHT = 2;

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

	// While most pipeline states in Vulkan need to be baked, we
	// can keep some stuff dynamic, such as the viewport and
	// scissor sizes
	const std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	// Validation layer
	const std::vector<const char*> validLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	class VulkanAPI : public GraphicAPI {
	private:
		// height/width
		int width, height;

		// Validation layers
#if DEBUG == 0
		const bool validLayer = false;
#else
		const bool validLayer = true;
#endif

		// Vulkan
		VkInstance vkInstance;
		VkPhysicalDevice vkPhysDevice;
		VkDevice vkDevice;

		// Queues
		VkQueue vkGraphicsQueue;
		VkQueue vkPresentQueue;

		// Surface
		VkSurfaceKHR vkSurface;

		// Swap Chain
		VkSwapchainKHR vkSwapChain;
		std::vector<VkImage> vkSwapChainImgs;
		VkFormat vkSwapChainImgFmt;
		VkExtent2D vkSwapChainExt;
		
		std::vector<VkImageView> vkSwapChainImgViews;

		// Prefered Swap Presentation mode
		VkPresentModeKHR vkPreferedSwapPresentationMode;

		// Dynamic state creation info
		VkPipelineDynamicStateCreateInfo dynamicState{};

		// Rendering stuff
		VkViewport vkViewport{};
		VkRect2D vkScissor{};
		VkPipelineViewportStateCreateInfo vkPipeLineViewPortCrInfo{};
		VkPipelineRasterizationStateCreateInfo vkRasterCrInfo{};
		VkPipelineMultisampleStateCreateInfo vkMultisampleCrInfo{};
		VkPipelineColorBlendAttachmentState vkColorBlendState{};
		VkPipelineColorBlendStateCreateInfo vkColorBlendCrInfo{};
		
		VkRenderPass vkRenderPass;
		VkPipelineLayout vkPipelineLayout;
		VkPipelineLayoutCreateInfo vkPipeLineLayoutCrInfo{};

		// Drawing stuff
		std::vector<VkFramebuffer> vkSwapChainFrameBuf;

		// Command pool
		VkCommandPool vkCommandPool;
		std::vector<VkCommandBuffer> vkCommandBuffer;

		// Staging buffer for vertices
		VkBuffer vkVertexStagingBuf;
		VkDeviceMemory vkVertexStageBufMem;
		// Our vertex buffer
		VkBufferCreateInfo vkBufCrInfo{};
		VkBuffer vkVertexBuffer;
		VkDeviceMemory vkVertexBufferMem;

		// Staging buffer for indices
		VkBuffer vkIndexStagingBuf;
		VkDeviceMemory vkIndexStageBufMem;
		// Our index buffer
		VkBuffer vkIndexBuffer;
		VkDeviceMemory vkIndexBufferMem;

		// Queuing and timing
		// A semaphore is a way to add order between GPU queue actions
		// its super cool
		std::vector<VkSemaphore> vkImageAvaSem;
		std::vector<VkSemaphore> vkRenderFinishedSem;
		// Fences are sorta the same thing but for the CPU
		std::vector<VkFence> vkInFlightFen;
		// occasionally, some drivers will NOT
		// trigger a VK_ERROR_OUT_OF_DATE_KHR when the 
		// window is resized. ew. so we gon handle it ourselves
		bool frameBufResize = false;

		uint32_t vkCurFrame = 0;

		// Internal Funcs
		// 
		// Check if validation layers are supported
		bool checkValidLayer();
		// Init stuff
		// Create VKInstance
		void vulkanCreateInstance();
		// Pick device to use
		void vulkanDevicePick();
		// Create logical device
		void vulkanCreateLogicDev();
		// Create Swap chain
		void vulkanCreateSwapChain();
		// Create image views
		void vulkanCreateImageViews();
		// Create render pass
		void vulkanCreateRenderPass();
		// Create graphics pipeline
		void vulkanCreateGraphicsPipeline();
		// Create frame buffers
		void vulkanCreateFramebuffers();
		// Create command pool
		void vulkanCreateCommandPool();
		// Create vertex buffer
		void vulkanCreateVertexBuffer();
		// Create index buffer
		void vulkanCreateIndexBuffer();
		// Create syncing objects
		void vulkanCreateSyncObjects();

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

		// Choose swap chain settings
		// Choose surface format
		VkSurfaceFormatKHR chooseSwapSurfFormat(const std::vector<VkSurfaceFormatKHR>& avaForm);
		// Choose present mode
		VkPresentModeKHR chooseSwapPresMode(const std::vector<VkPresentModeKHR>& avaPres);
		// Choose swap extent
		VkExtent2D chooseSwapExt(const VkSurfaceCapabilitiesKHR& caps);

		// Shaders
		VkShaderModule createShaderModule(const std::vector<char>& code);


		// Create buffer
		void vulkanCreateBuffer(VkDeviceSize devSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkBuffer& buffer, VkDeviceMemory& bufMem);
		// Copy buffer
		void vulkanCopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

		// Update vertex buffer
		void vulkanUpdateMeshBuffers(Nexus::Mesh* mesh);

		// Writes our commands to the command buffer
		void vulkanRecordCommandBuffer(uint32_t idx, VkPipeline grPipeline, size_t vert_size);

		// Recreate swap chain whenever anything bad happens
		void vulkanRecreateSwapChain();
		// we also need to clean up before recreating it
		void vulkanCleanSwapChain();

		// Get memory type
		uint32_t findMemType(uint32_t filter, VkMemoryPropertyFlags props);


		// Printing for debugging
		void debugPrint(std::string caller, std::string text, int level);

	public:
		VulkanAPI(GLFWwindow* window);

		void InitConnectionToWindow(GLFWwindow* window) override;
		void InitShaders(Scene* scene) override;	
		GLFWframebuffersizefun SetupWindowResize() override;
		void DrawFrame(Scene* scene) override;
		void CleanScene(Scene* scene) override;
		void Clean() override;

		// Getters and setters
		void setFBResize(bool n) { this->frameBufResize = n; }
		
	};


	static void frameBufResizeCallback(GLFWwindow* win, int w, int h);


	/*
	*/
	class VulkanShader : public GraphicsShader{
	public:
		VkShaderModule vert;
		VkShaderModule frag;
		VkPipelineShaderStageCreateInfo vertPipelineInfo, fragPipelineInfo;
		VkPipelineShaderStageCreateInfo shaderStages[2];
		VkPipelineVertexInputStateCreateInfo vertCrInfo{};
		VkPipelineInputAssemblyStateCreateInfo inputAsmCrInfo{};
		VkPipeline grPipeline;
	};
#else // VULKAN == 1
	// Dummy class if Vulkan is not enabled
	class VulkanAPI : public GraphicAPI {
	public:
		VulkanAPI(GLFWwindow* window) {

		}

		void InitConnectionToWindow(GLFWwindow* window) override;
		void InitShaders(Scene* scene) override;
		void DrawFrame(Scene* scene) override;
		void CleanScene(Scene* scene) override;
		void Clean() override;
	};
#endif
}