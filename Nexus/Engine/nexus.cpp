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
	renderer = new Renderer(gameProperties.gameName, gameProperties.windowHeight, gameProperties.windowWidth, Renderers(gameProperties.renderer));
}

void Nexus::Engine::InitScene(Scene* scene){
	renderer->getApi()->InitShaders(scene);
}

void Nexus::Engine::SetCamera(Camera* cam) {
	renderer->getApi()->SetCamera(cam);
}

void Nexus::Engine::Update(Scene* scene) {
	renderer->Tick(scene);
}


Nexus::Engine::~Engine() {

}