/*
*	RENDRER.CPP
*/
#include "renderer.h"

/*
*	Public
*/

Nexus::Renderer::Renderer(std::string title, int height, int width, Renderers render){
	this->height = height;
	this->width = width;

	// Initlize GLFW
	glfwInit();
	switch (render) {
	case RENDER_VULKAN:
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		break;
	case RENDER_GL:
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		// thanks steve jobs
		// who the hell is thanks?
		// glwindowhint
		// (that was super funny)
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		break;
	}
	// Create window
	window = glfwCreateWindow(height, width, title.c_str(), nullptr, nullptr);

	// Create Vulkan
	switch (render) {
	case RENDER_VULKAN:
		gApi = new VulkanAPI(window);
		break;
	case RENDER_GL:
		gApi = new OpenGLAPI(window);
		break;
	}

	glfwSetFramebufferSizeCallback(window, gApi->SetupWindowResize());
	
}

void Nexus::Renderer::Tick(Scene* scene) {
	prev = scene;
	if (glfwWindowShouldClose(window)) {
		// Currently just exit, in the future do something cool
		terminationTasks();
		exit(1);
	}
	// Check for minimization
	int w = 0, h = 0;
	glfwGetFramebufferSize(window, &w, &h);
	while (w == 0 && h == 0) {
		glfwGetFramebufferSize(window, &w, &h);
		glfwWaitEvents();
	}

	glfwPollEvents();
	gApi->DrawFrame(scene);
}


Nexus::Renderer::~Renderer() {
	std::cout << "Exiting Nexus Renderer!\n";
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
	if (prev) {
		gApi->CleanScene(prev);
	}
	gApi->Clean(); // Tell our current Graphics API to clean up after itself
	glfwDestroyWindow(window);
	glfwTerminate();
}