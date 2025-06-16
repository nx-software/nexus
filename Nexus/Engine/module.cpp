#include "module.h"

Nexus::Module::Module(std::string file) {
	// Attempt to load library
#ifdef _WIN32
	this->library = LoadLibraryA(file.c_str());

	if (this->library != NULL) {
		std::cout << file << " loaded successfully.\n";
	}
	else {
		Error(std::string{ "Nexus: Failed to load module " + file });
	}
#endif
}

void Nexus::Module::initClass(std::string name) {
#ifdef _WIN32
	// Ight 
	if ((CreateLibraryClass)GetProcAddress(library, "createClass")) {
		ModClass tmp;
		tmp.name = name;
		CreateLibraryClass crLib = (CreateLibraryClass)GetProcAddress(library, "createClass");
		tmp.c = crLib();
		this->classes.push_back(tmp);
	}
	else {
		Error(std::string{ "Nexus: Failed to load class " + name });
	}
#endif
}

void Nexus::Module::runFunction(std::string className, std::string functionName, int argCount, ...) {
#ifdef _WIN32
	for (auto& c : classes) {
		if (c.name == className) {
			// Ight lets get our func
			FunctionToRun func = (FunctionToRun)GetProcAddress(library, functionName.c_str());
			if (func) {
				va_list args;
				va_start(args, argCount);
				func(c.c, args);
				return;
			}
			else {
				Error(std::string{ "Nexus: Failed to load " + className + "::" + functionName });
			}
		}
	}
	Error(std::string{ "Nexus: Class " + className + " doesn't exist. Did you forget to load it?" });
#endif
}

Nexus::Module::~Module() {
#ifdef _WIN32
	FreeLibrary(library);
#endif
}