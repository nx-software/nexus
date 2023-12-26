/*
*	"Abstract" class for Graphics API's
*/
#pragma once

namespace Nexus {
	class GraphicAPI {
	private:
	public:
		GraphicAPI();

		virtual void Clean() = 0;
	};
}