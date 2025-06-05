#include "../Engine/mesh.h"

Nexus::Mesh::Mesh(std::vector<Vertex> verts, std::vector<uint16_t> inds) {
	this->vertices = verts;
	this->indices = inds;
	this->modified = true;
}