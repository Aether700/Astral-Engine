#include "aepch.h"
#include "VertexBuffer.h"
#include "AstralEngine/Platform/OpenGL/OpenGLVertexBuffer.h"
#include "RenderAPI.h"

namespace AstralEngine
{
	AReference<VertexBuffer> VertexBuffer::Create(unsigned int size, bool isInstanceArr)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:
			AE_CORE_ERROR("No RenderAPI is not yet supported");

		case RenderAPI::API::OpenGL:
			return AReference<OpenGLVertexBuffer>::Create(size, isInstanceArr);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}

	AReference<VertexBuffer> VertexBuffer::Create(float* data, unsigned int dataSize, bool isInstanceArr)
	{
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:
				AE_CORE_ERROR("No RenderAPI is not yet supported");

			case RenderAPI::API::OpenGL:
				return AReference<OpenGLVertexBuffer>::Create(data, dataSize, isInstanceArr);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}
}