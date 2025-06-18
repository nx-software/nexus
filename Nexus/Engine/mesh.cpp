#include "../Engine/mesh.h"

Nexus::Mesh::Mesh(std::vector<Vertex> verts, std::vector<uint16_t> inds) {
	this->vertices = verts;
	this->indices = inds;
	this->modified = true;
}

float* Nexus::Mesh::packVerts() {
	float* fVerts = (float*)malloc(this->getVertsRealSize());
	if (fVerts) {
		for (int i = 0; i < vertices.size(); i++) {
			int baseMod = i * 6;
			fVerts[baseMod] = vertices[i].pos.x;
			fVerts[1 + baseMod] = vertices[i].pos.y;
			fVerts[2 + baseMod] = vertices[i].pos.z;
			fVerts[3 + baseMod] = vertices[i].color.x;
			fVerts[4 + baseMod] = vertices[i].color.y;
			fVerts[5 + baseMod] = vertices[i].color.z;
		}
	}
	return fVerts;
}

unsigned int* Nexus::Mesh::packInds() {
	unsigned int* iVerts = (unsigned int*)malloc(sizeof(unsigned int) * indices.size());
	for (int i = 0; i < indices.size(); i++) {
		iVerts[i] = (unsigned int)indices[i];
	}
	return iVerts;
}