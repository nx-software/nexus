#pragma once

#include "gameObject.h"

#ifdef WIN32
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllexport)
#endif
#else
#define ENGINE_API
#endif

/*
* Special kind of gameobject for the camera! woaw
*/
namespace Nexus {
	/*class ENGINE_API Camera : public GameObject {
	private:
		typedef GameObject super;
	public:
		CameraData camData;

		Camera() : super("Camera") {}
	};*/
};