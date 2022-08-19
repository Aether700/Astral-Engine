#include "aepch.h"
#include "Shader.h"

#include "AstralEngine/Platform/OpenGL/OpenGLShader.h"
#include "RenderAPI.h"

namespace AstralEngine
{
	// Shader ////////////////////////////////////////////////////////////////////////////////////////////////////

	AReference<Shader> Shader::Create(const std::string& filepath)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:
			AE_CORE_ERROR("No RenderAPI is not yet supported");
			break;

		case RenderAPI::API::OpenGL:
			return AReference<OpenGLShader>::Create(filepath);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}
}