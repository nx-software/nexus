/*
* Handles Errors/Crashes
*/
#pragma once

#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif


namespace Nexus {
	class Error {
	private:
	public:
		Error(std::string text);
	};
}