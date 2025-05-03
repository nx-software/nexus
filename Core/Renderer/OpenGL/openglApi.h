/*
* Class for OpenGL rendering
*/
#pragma once

#include "../renderApi.h"
#include "../renderer.h"

namespace Nexus {
	class OpenGLAPI : public GraphicAPI {
	private:
	public:
		OpenGLAPI(GLFWwindow* window);

		void InitConnectionToWindow(GLFWwindow* window) override;
		void InitShaders(Scene* scene) override;
		void DrawFrame(Scene* scene) override;
		void CleanScene(Scene* scene) override;
		void Clean() override;
	};
}