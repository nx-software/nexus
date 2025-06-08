/*
    NX-Software Nexus 3D Engine
    audio.h - Optional DLL for managing audio
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

#include <cstdio>

#ifdef _WIN32
#ifdef AUDIO_EXPORTS
#define AUDIO_API __declspec(dllexport)
#else
#define AUDIO_API __declspec(dllexport)
#endif
#else
#define AUDIO_API
#endif

namespace Nexus{
    class AUDIO_API Audio {
    private:
    public:
        Audio();

        void initAudio();
    };
}