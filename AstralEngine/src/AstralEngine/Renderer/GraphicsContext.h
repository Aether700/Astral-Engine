#pragma once
#include "AstralEngine/Data Struct/AReference.h"

namespace AstralEngine
{
	class AWindow;

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() { }
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static AReference<GraphicsContext> Create(AWindow* window);
	};
}