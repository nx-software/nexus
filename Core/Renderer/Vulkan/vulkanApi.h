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

#ifdef __linux__
#include <xcb/xcb.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_xcb.h>
#include <X11/Xlib-xcb.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
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

	// While most pipeline states in Vulkan need to be baked, we
	// can keep some stuff dynamic, such as the viewport and
	// scissor sizes
	const std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	class VulkanAPI : public GraphicAPI {
	private:
		// height/width
		int width, height;
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
		VkPipelineColorBlendAttachmentState vkColorBlendState{};
		
		VkPipelineLayout vkPipelineLayout;
		VkPipelineLayoutCreateInfo vkPipeLineLayoutCrInfo{};

		// Internal Funcs
		// 
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
		// Create graphics pipeline
		void vulkanCreateGraphicsPipeline();

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
	


		void debugPrint(std::string caller, std::string text, int level);

	public:
		VulkanAPI(GLFWwindow* window);

		void InitConnectionToWindow(GLFWwindow* window) override;
		void InitShaders(Scene* scene) override;	
		void CleanScene(Scene* scene) override;
		void Clean();
		
	};

	class VulkanShader : public GraphicsShader{
	public:
		VkShaderModule vert;
		VkShaderModule frag;
		VkPipelineShaderStageCreateInfo vertPipelineInfo, fragPipelineInfo;
		VkPipelineVertexInputStateCreateInfo vertCrInfo{};
		VkPipelineInputAssemblyStateCreateInfo inputAsmCrInfo{};
	};
}