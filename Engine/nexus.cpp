/*
* NEXUS.CPP
*/

// Nexus
#include "nexus.h"


Nexus::Engine::Engine(GameProperties gameProperties) {
	// Init logging
#if LOGGING == 1
	static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("nexusLog.txt");
	static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
	plog::init(plog::debug, &fileAppender).addAppender(&consoleAppender);

	PLOG_DEBUG << "Nexus Init - Version " << MAJOR_VER_NUM << "." << MINOR_VER_NUM;
#endif
	// Init Renderer
	renderer = new Renderer(gameProperties.gameName, gameProperties.windowHeight, gameProperties.windowWidth);
}

void Nexus::Engine::Update(Scene* scene) {
	renderer->Tick();
}


Nexus::Engine::~Engine() {

}