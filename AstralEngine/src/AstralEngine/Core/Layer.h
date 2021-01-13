#pragma once
#include "AstralEngine/AEvents/AEvent.h"

namespace AstralEngine
{
	class Layer
	{
	public:
		Layer() { }
		virtual ~Layer() { }
		
		virtual void OnUpdate() { }
		virtual bool OnEvent(AEvent& e) { return false; }
		virtual void OnAttach() { }
		virtual void OnDetach() { }

	};
}