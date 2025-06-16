#include "openglApi.h"

Nexus::OpenGLAPI::OpenGLAPI(GLFWwindow* window) {
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Error("OpenGL: Failed to init GLAD!");
	}

	// Get window size
	glfwGetWindowSize(window, &width, &height);

	// Setup view port
	glViewport(0, 0, width, height);

	this->window = window;
	printf("OpenGL init!\n");
}

void Nexus::OpenGLAPI::InitConnectionToWindow(GLFWwindow* window) {

}
GLFWframebuffersizefun Nexus::OpenGLAPI::SetupWindowResize() {
	return nullptr;
}
void Nexus::OpenGLAPI::InitShaders(Scene* scene) {
	for (auto& gm : scene->getObjects()) {
		// Compile vertex shader
		unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
		const char* vertexCode = generateStringFromArrayShaderCode(gm->getVertShader()->readShader()).c_str();
		glShaderSource(vShader, 1, &vertexCode, NULL);
	}
}
void Nexus::OpenGLAPI::DrawFrame(Scene* scene) {
	// Setup clear color
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

}
void Nexus::OpenGLAPI::CleanScene(Scene* scene) {

}
void Nexus::OpenGLAPI::Clean() {

}

std::string Nexus::OpenGLAPI::generateStringFromArrayShaderCode(std::vector<char> characters) {
	std::string code;
	for (char c : characters) {
		code += c;
	}
	return code;
}