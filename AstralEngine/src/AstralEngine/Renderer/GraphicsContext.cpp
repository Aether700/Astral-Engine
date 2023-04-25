#include "aepch.h"
#include "GraphicsContext.h"
#include "AstralEngine/Platform/OpenGL/OpenGLGraphicsContext.h"
#include "RenderAPI.h"

namespace AstralEngine
{
	AReference<GraphicsContext> GraphicsContext::Create(AWindow* window)
	{
		switch(RenderAPI::GetAPI())
		{
			case RenderAPI::API::OpenGL:
				return AReference<OpenGLGraphicsContext>::Create(window);
		}

		AE_CORE_ERROR("Unknown RenderAPI detected during creation of graphics context");
		return nullptr;
	}
}