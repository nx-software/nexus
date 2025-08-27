// Putting this here 
// because MSVC doesn't like
// texture.h having this in it??
#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"

Nexus::Texture::Texture(std::string filePath) {
	this->pixels = stbi_load(filePath.c_str(), &tWidth, &tHeight, &tChannels, STBI_rgb_alpha);
	if (!pixels) {
		Error(std::string{ "Nexus: Failed to load texture " + filePath });
	}
}