#include "openglApi.h"

Nexus::OpenGLAPI::OpenGLAPI(GLFWwindow* window) {
	glfwMakeContextCurrent(window);
	// Set user pointer
	glfwSetWindowUserPointer(window, this);

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
	return Nexus::setupOpenGLFrameBufferCallback;
}

void Nexus::OpenGLAPI::InitShaders(Scene* scene) {
	for (auto& gm : scene->getObjects()) {
		// Compile vertex shader
		unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
		std::string vCodeInter = generateStringFromArrayShaderCode(gm->getVertShader()->readSource());
		const char* vertexCode = vCodeInter.c_str();
		glShaderSource(vShader, 1, &vertexCode, NULL);
		glCompileShader(vShader);
		// Did it compile correctly...
		checkShaderComp(vShader);
		// Compile fragment shader
		unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);
		std::string fCodeInter = generateStringFromArrayShaderCode(gm->getFragShader()->readSource());
		const char* fragmentCode = fCodeInter.c_str();
		glShaderSource(fShader, 1, &fragmentCode, NULL);
		glCompileShader(fShader);
		// Did it compile correctly...
		checkShaderComp(fShader);

		// Shader program !
		unsigned int sProgram = glCreateProgram();
		glAttachShader(sProgram, vShader);
		glAttachShader(sProgram, fShader);
		glLinkProgram(sProgram);
		// Did it error out...
		int suc;
		char log[512];
		glGetProgramiv(sProgram, GL_LINK_STATUS, &suc);
		if (!suc) {
			glGetProgramInfoLog(sProgram, sizeof(log), NULL, log);
			Error(std::string{ "OpenGL: Error linking shader program, log: " + std::string{log} });
		}
		glDeleteShader(vShader);
		glDeleteShader(fShader);
		// Add it to our newly created OpenGL shader !
		OpenGLShader glShader;
		glShader.shaderProgram = sProgram;

		// Lets get our data in !
		glGenVertexArrays(1, &(glShader.VAO));
		glGenBuffers(1, &(glShader.VBO));
		glGenBuffers(1, &(glShader.EBO));
		// bind the Vertex Array Object
		glBindVertexArray(glShader.VAO);

		// Bind the Vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, glShader.VBO);
		glBufferData(GL_ARRAY_BUFFER, gm->mesh->getVertsRealSize(), gm->mesh->getFloatVerts(), GL_STATIC_DRAW);

		// Bind the Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glShader.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, gm->mesh->getIndsRealSize(), gm->mesh->getFloatInd(), GL_STATIC_DRAW);

		printf("====Vertex Info====\nSize of Vertex array: %d | Size of Index Array: %d\nData: %f %d\n", gm->mesh->getVertsRealSize(), gm->mesh->getIndsRealSize(), gm->mesh->getFloatVerts()[4], gm->mesh->getFloatInd()[1]);
		
		float* vals = gm->mesh->getFloatVerts();
		printf("%d\n", gm->mesh->getVertsRealSize());
		for (int i = 0; i < gm->mesh->getVertsRealSize() / sizeof(float); i++) {
			printf("%f ", vals[i]);
		}

		// Attributes!
		// Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);
		// Color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		// Bind our vertex array
		glBindVertexArray(0);

		gm->gShader = new OpenGLShader(glShader);
	}
}

void Nexus::OpenGLAPI::DrawFrame(Scene* scene) {
	// Setup clear color
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render each object
	for (auto& gm : scene->getObjects()) {
		OpenGLShader* s = (OpenGLShader*)gm->gShader;
		// Use shader program
		glUseProgram(s->shaderProgram);
		// Setup camera
		int modelLocation = glGetUniformLocation(s->shaderProgram, "model");
		int viewLocation = glGetUniformLocation(s->shaderProgram, "view");
		int projLocation = glGetUniformLocation(s->shaderProgram, "proj");
		// Set values within shader
		glm::mat4 identity = glm::mat4(1.0f);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cam->camData.model));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(cam->camData.view));
		// Projection matrix... lets set it up
		cam->camData.proj = glm::perspective(glm::radians((float)cam->pAngle), (float)width / (float)height, 0.1f, 100.0f);
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(cam->camData.proj));
		// Draw
		glBindVertexArray(s->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->EBO);
		glDrawElements(GL_TRIANGLES, gm->mesh->getIndsRealSize(), GL_UNSIGNED_INT, 0);
	}

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

void Nexus::OpenGLAPI::checkShaderComp(unsigned int shader) {
	int suc;
	char log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &suc);
	if (!suc) {
		glGetShaderInfoLog(shader, sizeof(log), NULL, log);
		Error(std::string{ "OpenGL: Error compiling shader, log: " + std::string{log} });
	}
}

void Nexus::OpenGLAPI::resizeWindow() {
	// Update window size
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
}

static void Nexus::setupOpenGLFrameBufferCallback(GLFWwindow* win, int w, int h) {
	auto glApi = reinterpret_cast<OpenGLAPI*>(glfwGetWindowUserPointer(win));
	glApi->resizeWindow();
}