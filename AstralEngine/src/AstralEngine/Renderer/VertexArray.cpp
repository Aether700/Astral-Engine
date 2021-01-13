#include "aepch.h"
#include "VertexArray.h"
#include "AstralEngine/Platform/OpenGL/OpenGLVertexArray.h"
#include "RenderAPI.h"

namespace AstralEngine
{
	AReference<VertexArray> VertexArray::Create()
	{
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:
				AE_CORE_ERROR("No RenderAPI is not yet supported");

			case RenderAPI::API::OpenGL:
				return AReference<OpenGLVertexArray>::Create();
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}
}