/*
	NX-Software Nexus 3D Engine
	texture.h - Header for loading and working with textures
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

#ifdef _WIN32
#ifdef RENDERER_EXPORTS
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllexport)
#endif
#else
#define RENDERER_API
#endif

#include "../../CrashHandler/CrashHandler.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>

namespace Nexus {
	class Texture {
	private:
		int tWidth, tHeight, tChannels;
		stbi_uc* pixels;
	public:
		RENDERER_API Texture(std::string filePath);

		// Getters
		RENDERER_API int getWidth() {
			return tWidth;
		}

		RENDERER_API int getHeight() {
			return tHeight;
		}

		RENDERER_API int getChannels() {
			return tChannels;
		}
	};
}