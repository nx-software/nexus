/*
* NEXUS.CPP
*/

// Nexus
#include "nexus.h"


Nexus::Engine::Engine(GameProperties gameProperties) {
	renderer = new Renderer(gameProperties.gameName, gameProperties.windowHeight, gameProperties.windowWidth);
}

void Nexus::Engine::Update() {
	renderer->Tick();
}