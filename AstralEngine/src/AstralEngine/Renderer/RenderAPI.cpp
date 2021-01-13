#include "aepch.h"
#include "RenderAPI.h"
#include "AstralEngine/Platform/OpenGL/OpenGLRenderAPI.h"

namespace AstralEngine
{
	RenderAPI* RenderAPI::Create()
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:
			AE_CORE_ERROR("No RenderAPI is not yet supported");

		case RenderAPI::API::OpenGL:
			return new OpenGLRenderAPI();
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}
}