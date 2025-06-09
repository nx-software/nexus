#include "audio.h"

Nexus::Audio::Audio() {

}

void Nexus::Audio::initAudio() {
	printf("Audio! From another DLL!\n");
}

extern "C" AUDIO_API Nexus::Audio* createClass() {
	return new Nexus::Audio();
}

extern "C" AUDIO_API void initAudio(Nexus::Audio* instance) {
	instance->initAudio();
}