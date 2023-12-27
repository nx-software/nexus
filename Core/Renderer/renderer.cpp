/*
*	RENDRER.CPP
*/
#include "renderer.h"

/*
*	Public
*/

Nexus::Renderer::Renderer(std::string title, int height, int width) {
	this->height = height;
	this->width = width;

	// Initlize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Create window
	window = glfwCreateWindow(height, width, title.c_str(), nullptr, nullptr);

	// Create Vulkan
	gApi = new VulkanAPI(window);
}

void Nexus::Renderer::Tick() {
	if (glfwWindowShouldClose(window)) {
		// Currently just exit, in the future do something cool
		terminationTasks();
		exit(1);
	}

	glfwPollEvents();
}


Nexus::Renderer::~Renderer() {
	// i pick up phone
	// nexus is kill
	// no
	// (haha laugh at dead meme from like 7 years go haha)
	terminationTasks();
}

/*
*	Internal
*/


void Nexus::Renderer::terminationTasks() {
	gApi->Clean();
	glfwDestroyWindow(window);
	glfwTerminate();
}