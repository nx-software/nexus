/*
*/
#include "CrashHandler.h"


Nexus::Error::Error(std::string text) {
	std::string title = "Nexus: Fatal Error!";
	
	// Process for displaying windows/errors varies by system

	// Windows
#ifdef _WIN32
	MessageBox(NULL, std::wstring(text.begin(), text.end()).c_str(), std::wstring(title.begin(), title.end()).c_str(), MB_OK | MB_ICONERROR);
#endif

	// We done, exit
	exit(-1);
}