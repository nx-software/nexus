/*
* NEXUS Game Engine
*/
#pragma once
// Internal Files
#include "../Core/Renderer/renderer.h"
#include "scene.h"
#include "config.h"

#include <iostream>



#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>

// Everything is in the namespace Nexus 
namespace Nexus {

#define MAJOR_VER_NUM 0
#define MINOR_VER_NUM 0

	class GameProperties {
	public:
		std::string gameName;
		
		int windowHeight;
		int windowWidth;
	};

	/*
	* Class for the base engine
	*/
	class Engine {
	private:
		std::string name;

		// Engine components
		Renderer* renderer;
	public:
		Engine(GameProperties);

		void Update(Scene* scene);

		~Engine();
	};
}

