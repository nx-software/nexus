/*
* Handles Errors/Crashes
*/
#pragma once

#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

namespace Nexus {
	class Error {
	private:
	public:
		Error(std::string text);
	};
}