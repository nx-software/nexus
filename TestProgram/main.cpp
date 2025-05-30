/*
*	Demo of NEXUS
* 
*	Files like this will prob be generated by some sort
*	of Nexus GUI editor
*/
#include "../Engine/nexus.h"
#include "../Engine/gameObject.h"
#include "../Engine/scene.h"

int main() {
	// Create properties so Nexus knows whats up
	Nexus::GameProperties gm;
	gm.gameName = "Nexus Test";
	gm.windowHeight = 800;
	gm.windowWidth = 600;

	Nexus::Engine* engine = new Nexus::Engine(gm);

	Nexus::Scene testScene("Test Scene");

	Nexus::GameObject gameObject("Test Object");

	gameObject.loadVertShader("../shaders/vert.spv");
	gameObject.loadFragShader("../shaders/frag.spv");

	testScene.addObject(&gameObject);

	engine->InitScene(&testScene);

	while (1) {
		engine->Update(&testScene);
	}
}