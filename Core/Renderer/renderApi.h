/*
*	"Abstract" class for Graphics API's
*/
#pragma once
#include "../../CrashHandler/CrashHandler.h"

namespace Nexus {
	class GraphicAPI {
	private:
	public:
		GraphicAPI();

		virtual void Clean() = 0;
	};
}