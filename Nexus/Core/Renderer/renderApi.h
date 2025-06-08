/*
	NX-Software Nexus 3D Engine
	renderApi.h - Abstract class for graphic API implementation
	Copyright (C) 2025 Electro-Corp

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once
#include "../../CrashHandler/CrashHandler.h"
#include "../../Engine/scene.h"
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef _WIN32
// If we on windows this is prob important
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

namespace Nexus {
	struct CameraData {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	class Camera {
	public:
		CameraData camData;
		double pAngle;
	};
	
	typedef struct GraphicsCard{
		std::string name;
	};
	
	class GraphicAPI {
	private:
	protected:
		GraphicsCard chosenGraphicsCard;
		Camera* cam;
	public:
		GraphicAPI();

		void SetCamera(Camera* cam) {
			this->cam = cam;
		}

		virtual void InitConnectionToWindow(GLFWwindow* window) = 0;
		virtual GLFWframebuffersizefun SetupWindowResize() = 0;
		virtual void InitShaders(Scene* scene) = 0;

		virtual void DrawFrame(Scene* scene) = 0;
		GraphicsCard getGraphicsCard();

		virtual void CleanScene(Scene* scene) = 0;
		virtual void Clean() = 0;
	};


	// Holder for the seperate graphics things that the 
	// different graphics APIs have
	class GraphicsShader{
	private:
	public:
		GraphicsShader();
	};
}