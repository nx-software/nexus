#include "module.h"

Nexus::Module::Module(std::string file) {
	// Attempt to load library
#ifdef _WIN32
	this->library = LoadLibraryA(file.c_str());

	if (this->library != NULL) {
		std::cout << file << " loaded successfully.";
	}
	else {
		Error(std::string{ "Nexus: Failed to load module " + file });
	}
#endif
}