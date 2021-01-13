#include "aepch.h"
#include "Framebuffer.h"
#include "RenderAPI.h"
#include "AstralEngine/Platform/OpenGL/OpenGLFramebuffer.h"

namespace AstralEngine
{
	AReference<Framebuffer> Framebuffer::Create(unsigned int width, unsigned int height, bool isSwapChainTarget)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:
			AE_CORE_ERROR("No RenderAPI is not yet supported");

		case RenderAPI::API::OpenGL:
			return AReference<OpenGLFramebuffer>::Create(width, height, isSwapChainTarget);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}
}