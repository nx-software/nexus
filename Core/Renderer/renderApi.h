/*
*	"Abstract" class for Graphics API's
*/
#pragma once
#include "../../CrashHandler/CrashHandler.h"
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef _WIN32
// If we on windows this is prob important
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

namespace Nexus {
	class GraphicAPI {
	private:
	public:
		GraphicAPI();

		virtual void InitConnectionToWindow(GLFWwindow* window) = 0;

		virtual void Clean() = 0;
	};
}