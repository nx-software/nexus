/*
    NX-Software Nexus 3D Engine
    openglApi.h - OpenGL Rendering support
    Copyright (C) 2025 Electro-Corp

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "../renderApi.h"
#include "../renderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Nexus {
    class OpenGLShader : public GraphicsShader {
    public:
        unsigned int shaderProgram;
        /*
         *  VAO -> Tells OpenGL how our vertices are laid out
         *  VBO -> for our vertices
         *  EBO -> for our indices
         */
        unsigned int VBO, VAO, EBO;
    };

	class OpenGLAPI : public GraphicAPI {
	private:
        // Width and height of window
        int width, height;
        // Acutal GLFW window
        GLFWwindow* window;

        // Internal funcs
        // Take the vector of characters from the shader and convert into something 
        // OpenGL likes
        std::string generateStringFromArrayShaderCode(std::vector<char> chars);

        // Check shader comp 
        void checkShaderComp(unsigned int shader);
	public:
		OpenGLAPI(GLFWwindow* window);

		void InitConnectionToWindow(GLFWwindow* window) override;
		GLFWframebuffersizefun SetupWindowResize() override;
		void InitShaders(Scene* scene) override;
		void DrawFrame(Scene* scene) override;
		void CleanScene(Scene* scene) override;
		void Clean() override;

        // Resize window
        void resizeWindow();
	};

    static void setupOpenGLFrameBufferCallback(GLFWwindow* win, int w, int h);
}