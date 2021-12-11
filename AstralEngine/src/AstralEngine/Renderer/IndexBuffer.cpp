#include "aepch.h"
#include "IndexBuffer.h"
#include "AstralEngine/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "RenderAPI.h"


namespace AstralEngine
{
	AReference<IndexBuffer> IndexBuffer::Create()
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:
			AE_CORE_ERROR("No RenderAPI is not yet supported");

		case RenderAPI::API::OpenGL:
			return AReference<OpenGLIndexBuffer>::Create();
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}

	AReference<IndexBuffer> IndexBuffer::Create(unsigned int* indices, unsigned int count)
	{
		switch(RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:
				AE_CORE_ERROR("No RenderAPI is not yet supported");

			case RenderAPI::API::OpenGL:
				return AReference<OpenGLIndexBuffer>::Create(indices, count);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}
}