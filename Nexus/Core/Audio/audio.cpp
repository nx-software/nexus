#include "audio.h"

Nexus::Audio::Audio() {

}

void Nexus::Audio::initAudio() {
	printf("Audio! From another DLL!\n");
}

void Nexus::Audio::playAudio(std::string file) {
	printf("Audio playing %s\n", file.c_str());
}

extern "C" AUDIO_API Nexus::Audio* createClass() {
	return new Nexus::Audio();
}

extern "C" AUDIO_API void initAudio(Nexus::Audio* instance) {
	instance->initAudio();
}

extern "C" AUDIO_API void playAudio(Nexus::Audio* instance, va_list args) {
	instance->playAudio(va_arg(args, std::string));
}