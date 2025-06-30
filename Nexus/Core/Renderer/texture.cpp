#include "texture.h"

Nexus::Texture::Texture(std::string filePath) {
	this->pixels = stbi_load(filePath.c_str(), &tWidth, &tHeight, &tChannels, STBI_rgb_alpha);
	if (!pixels) {
		Error(std::string{ "Nexus: Failed to load texture " + filePath });
	}
}