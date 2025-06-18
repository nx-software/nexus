/*
*/
#include "CrashHandler.h"


CRASH_HANDLER_API Nexus::Error::Error(std::string text) {
	std::string title = "Nexus: ";
#if ENABLE_RANDOM_TITLEBAR == 1
	srand(time(0));
	int msg = rand() % messages.size();
	title += messages[msg];
#else
	title += "Fatal Error!"
#endif

	std::cout << title << " " << text << "\n";
	
	// Process for displaying windows/errors varies by system

	// Windows
#ifdef _WIN32
	MessageBox(NULL, std::wstring(text.begin(), text.end()).c_str(), std::wstring(title.begin(), title.end()).c_str(), MB_OK | MB_ICONERROR);
#endif
#ifdef __linux__
	Display* disp = NULL;
	disp = XOpenDisplay(0);
	int ds = DefaultScreen(disp);
#endif
	// We done, exit
	exit(-1);
}