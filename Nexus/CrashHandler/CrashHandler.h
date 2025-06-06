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

#ifdef _WIN32
#ifdef CRASHHANDLER_EXPORTS
#define CRASH_HANDLER_API __declspec(dllexport)
#else
#define CRASH_HANDLER_API __declspec(dllexport)
#endif
#else
#define CRASH_HANDLER_API
#endif

namespace Nexus {
	class Error {
	private:
	public:
		CRASH_HANDLER_API Error(std::string text);
	};
}