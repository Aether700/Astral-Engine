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

		case RenderAPI::API::OpenGL:
			return AReference<OpenGLShader>::Create(filepath);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}

	AReference<Shader> Shader::Create(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc)
	{
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:
				AE_CORE_ERROR("No RenderAPI is not yet supported");

			case RenderAPI::API::OpenGL:
				return AReference<OpenGLShader>::Create(name, vertexShaderSrc, fragmentShaderSrc);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}

	// ShaderLibrary //////////////////////////////////////////////////////////////////////////////////////////////////

	
	void ShaderLibrary::Add(const std::string& name, AReference<Shader> shader)
	{
		AE_PROFILE_FUNCTION();
		AE_CORE_ASSERT(!Exists(name), "A Shader with the name '%S' already exists", name);
		m_shaders[name] = shader;
	}
	
	void ShaderLibrary::Add(AReference<Shader> shader)
	{
		Add(shader->GetName(), shader);
	}
	
	AReference<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		AReference<Shader> shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}
	
	AReference<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		AReference<Shader> shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}
	
	AReference<Shader> ShaderLibrary::Get(const std::string& name)
	{
		AE_CORE_ASSERT(Exists(name), "Shader '%S' could be found", name);
		return m_shaders[name];
	}
	
	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_shaders.ContainsKey(name);
	}

}