#include "audio.h"

Nexus::Audio::Audio() {

}

void Nexus::Audio::initAudio() {
	printf("Audio! From another DLL!\n");
}

void Nexus::Audio::playAudio(const char* file) {
	printf("Audio playing %s\n", file);
}

//
// Module C styel accessors 
//

extern "C" AUDIO_API Nexus::Audio* createClass() {
	return new Nexus::Audio();
}

extern "C" AUDIO_API void initAudio(Nexus::Audio* instance) {
	instance->initAudio();
}

extern "C" AUDIO_API void playAudio(Nexus::Audio* instance, va_list args) {
	const char* filePath = va_arg(args, const char*);
	instance->playAudio(filePath);
}