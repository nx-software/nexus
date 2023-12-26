/*
* NEXUS Game Engine
*/
#pragma once
// Internal Files
#include "../Core/Renderer/renderer.h"

#include <iostream>

// Everything is in the namespace Nexus 
namespace Nexus {
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

		void Update();

		~Engine();
	};
}

