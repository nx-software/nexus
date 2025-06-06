/*
	NX-Software Nexus 3D Engine
	renderer.h - Handles definition of current rendering APIs and scenes
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

#ifdef _WIN32
#ifdef RENDERER_EXPORTS
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllexport)
#endif
#else
#define RENDERER_API
#endif

// Includes 
#include "renderApi.h"
#include "Vulkan/vulkanApi.h"
#include "OpenGL/openglApi.h"
// We use Vulkan so
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include <iostream>

namespace Nexus {	
	class GraphicAPI;

	enum RENDERER_API Renderers {
		RENDER_VULKAN,
		RENDER_GL,
		RENDER_DX
	};

	class Renderer {
	private:
		GLFWwindow* window;
		int height, width;

		
		// API
		GraphicAPI* gApi;
		GraphicsCard chosenCard;

		Scene* prev;

		// Stuff to do when kill
		void terminationTasks();

	public:
		// Create Renderer
		RENDERER_API Renderer(std::string title, int height, int width, Renderers render);

		RENDERER_API GraphicAPI* getApi(){ return this->gApi; }

		// Tick
		RENDERER_API void Tick(Scene* scene);

		~Renderer();
	};
}