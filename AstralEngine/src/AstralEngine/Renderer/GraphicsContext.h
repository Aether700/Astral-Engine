#pragma once
#include "AstralEngine/Data Struct/AReference.h"

namespace AstralEngine
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() { }
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}