/*
* NEXUS.CPP
*/

// Nexus
#include "nexus.h"


Nexus::Engine::Engine(GameProperties gameProperties) {
	// Grab name of selected renderer
	std::string selectedRenderApi;
	switch(gameProperties.renderer){
		case 0:
			selectedRenderApi = "Vulkan";
			break;
		case 1:
			selectedRenderApi = "OpenGL";
			break;
		default:
			selectedRenderApi = "Unknown";
			break;
	}
	// Init logging
#if LOGGING == 1
	static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("nexusLog.txt");
	static plog::ColorConsoleAppender<plog::TxtFormatter> colorConsoleAppender;
	plog::init(plog::debug, &fileAppender).addAppender(&colorConsoleAppender);

	PLOG_DEBUG << "NX-Software Nexus Engine - Version " << MAJOR_VER_NUM << "." << MINOR_VER_NUM;
	PLOG_DEBUG << "(c) NX-Software {@Electro-Corp} " << COPYRIGHT_YEARS;
	PLOG_DEBUG << "Selected Renderer: " << selectedRenderApi << "\n";
#endif
	// Init Renderer
	renderer = new Renderer(std::string{gameProperties.gameName + " | " + selectedRenderApi}, gameProperties.windowHeight, gameProperties.windowWidth, Renderers(gameProperties.renderer));
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