#include "aepch.h"
#include "Shader.h"

#include "AstralEngine/Platform/OpenGL/OpenGLShader.h"
#include "RenderAPI.h"

namespace AstralEngine
{
	// Shader ////////////////////////////////////////////////////////////////////////////////////////////////////

	ShaderHandle Shader::DefaultShader()
	{
		static ShaderHandle defaultShader = ResourceHandler::LoadShader("assets/shaders/DefaultShader.glsl");
		return defaultShader;
	}

	ShaderHandle Shader::SpriteShader()
	{
		static ShaderHandle sprite = ResourceHandler::LoadShader("assets/shaders/SpriteShader.glsl");
		return sprite;
	}

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