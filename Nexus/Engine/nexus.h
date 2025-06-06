/*
	NX-Software Nexus 3D Engine
	nexus.h - Root of the Nexus engine
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
#include <iostream>

// Internal Files
#include "../Core/Renderer/renderer.h"
#include "scene.h"
#include "config.h"

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>

#ifdef _WIN32
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllexport)
#endif
#else
#define ENGINE_API
#endif

// Everything is in the namespace Nexus 
namespace Nexus {

#define MAJOR_VER_NUM 0
#define MINOR_VER_NUM 0

	class ENGINE_API GameProperties {
	public:
		std::string gameName;
		
		int windowHeight;
		int windowWidth;
	};

	/*
	* Class for the base engine
	*/
	class ENGINE_API Engine {
	private:
		std::string name;

		// Engine components
		Renderer* renderer;
	public:
		Engine(GameProperties);

		void InitScene(Scene* scene);

		void SetCamera(Camera* cam);

		void Update(Scene* scene);

		~Engine();
	};
}