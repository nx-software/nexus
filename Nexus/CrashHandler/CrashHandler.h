/*
    NX-Software Nexus 3D Engine
    CrashHandler.h - Support for showing error messages cross platform
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

#include <iostream>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

#ifdef _WIN32
#ifdef CRASHHANDLER_EXPORTS
#define CRASH_HANDLER_API __declspec(dllexport)
#else
#define CRASH_HANDLER_API __declspec(dllexport)
#endif
#else
#define CRASH_HANDLER_API
#endif

// Enable various titlebar messages
#define ENABLE_RANDOM_TITLEBAR 1

namespace Nexus {
	class Error {
	private:
#if ENABLE_RANDOM_TITLEBAR == 1
        std::vector<std::string> messages = {
            "Look who's incompetent...",
            "The kid who dreamed of being a programmer...",
            "You messed up bad!",
            "Your graphics card is sobbing.",
            "Did you think that would work?",
            "Pfft. Course it crashed. An AP CSA student could predict that.",
            "Aw.. do you need me to do it for you?",
            "It's nice that you tired!",
            "Give up.",
            "Fatal Error!"
        };
#endif
	public:
		CRASH_HANDLER_API Error(std::string text);
	};
}