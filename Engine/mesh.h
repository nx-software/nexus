/*
 * Header for general raw meshes
 */
#pragma once

#include <array>

#include <glm/glm.hpp>

#include "config.h"
#include "../Core/Renderer/renderApi.h"



namespace Nexus {
	// Structure holding one vertex
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;
		
#if VULKAN == 1
		static VkVertexInputBindingDescription getBindDesc() {
			VkVertexInputBindingDescription  bindDesc{};
			bindDesc.binding = 0; // index of bindings
			bindDesc.stride = sizeof(Vertex); // number of bytes till next vert
			bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Since we're not using instanced rendering yet, 
															  // just use per-vertex data
			return bindDesc;
		}

		/*
		*	Tells us how to handle the vertex input
		*/
		static std::array<VkVertexInputAttributeDescription, 2> getAttrDesc() {
			std::array< VkVertexInputAttributeDescription, 2> attrDescs{};
			attrDescs[0].binding = 0; // Which binding the data is coming from
			attrDescs[0].location = 0; // Refrences the "location = 0" from our vertex shader
			attrDescs[0].format = VK_FORMAT_R32G32_SFLOAT; // oopsie here comes a lesson
			// Format for data, here are the common bindings:
			// float = VK_FORMAT_R32_SFLOAT
			// vec2 = VK_FORMAT_R32G32_SFLOAT
			// vec3 = VK_FORMAT_R32G32B32_SFLOAT
			// vec4 = VK_FORMAT_R32G32B32A32_SFLOAT
			// as you can surmise, we're using a vec2. But wait, aren't these color values?
			// Yeah.. but lowk just choose the one that has the same amount of data as our vertex input
			attrDescs[0].offset = offsetof(Nexus::Vertex, pos);

			// Same thing but for color
			attrDescs[1].binding = 0;
			attrDescs[1].location = 1; // Refrences the "location = 1" from our vertex shader
			attrDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT; // look above
			attrDescs[1].offset = offsetof(Nexus::Vertex, color);

			return attrDescs;
		}
#endif
	};

	// Class to hold vertices and indices (thats basically it)
	class Mesh {
	private:
		// Pretty self explanatory, just the ends of our triangle
		std::vector<Vertex> vertices;
		/*
		 * Ooh goody optimization
		 * Often, triangles will share multiple verticies, as shown here in this simple example:
		 *      v0 --------------- v1
		 *     v5 | *              |
		 *        |     *          |
		 *        |         *      |
		 *        |             *  |
		 *      v4 ---------------- v2
		 *                         v3
		 *  So why bother using that many verts when v0 and v5 are the same and v3 and v2 are?
		 *  Yay for index buffers
		 */
		std::vector<uint16_t> indices;
	public:
		// Has our mesh been modified?
		bool modified = false;

		Mesh(std::vector<Vertex> verts, std::vector<uint16_t> inds);

		// Getters/Setters
		std::vector<Vertex> getVertices() {
			return vertices;
		}

		void setVertices(std::vector<Vertex> verts) {
			this->vertices = verts;
			modified = true;
		}

		std::vector<uint16_t> getIndicies() {
			return indices;
		}

		void setIndicies(std::vector<uint16_t> inds) {
			this->indices = inds;
			modified = true;
		}
	};
}