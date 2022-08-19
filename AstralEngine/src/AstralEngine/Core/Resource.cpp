#include "aepch.h"
#include "Resource.h"
#include "AstralEngine/Renderer/Texture.h"
#include "AstralEngine/Renderer/Shader.h"
#include "AstralEngine/Renderer/Renderer.h"

namespace AstralEngine
{
	// ResourceHandler /////////////////////////////////////////////////////////////
	ShaderHandle ResourceHandler::LoadShader(const std::string& filepath)
	{
		return GetHandler()->m_shaders.AddResource(Shader::Create(filepath));
	}

	AReference<Shader> ResourceHandler::GetShader(ShaderHandle handle)
	{
		return GetHandler()->m_shaders.GetResource(handle);
	}

	Texture2DHandle ResourceHandler::LoadTexture2D(const std::string& filepath)
	{
		return GetHandler()->m_textures2D.AddResource(Texture2D::Create(filepath));
	}
	
	Texture2DHandle ResourceHandler::LoadTexture2D(unsigned int width, unsigned int height)
	{
		return GetHandler()->m_textures2D.AddResource(Texture2D::Create(width, height));
	}

	Texture2DHandle ResourceHandler::LoadTexture2D(unsigned int width, unsigned int height,
		void* data, unsigned int size)
	{
		return GetHandler()->m_textures2D.AddResource(Texture2D::Create(width, height, data, size));
	}

	AReference<Texture2D> ResourceHandler::GetTexture2D(Texture2DHandle handle)
	{
		return GetHandler()->m_textures2D.GetResource(handle);
	}

	MaterialHandle ResourceHandler::CreateMaterial()
	{
		return GetHandler()->m_materials.AddResource(AReference<Material>::Create());
	}

	MaterialHandle ResourceHandler::CreateMaterial(const Vector4& color)
	{
		return GetHandler()->m_materials.AddResource(AReference<Material>::Create(color));
	}

	AReference<Material> ResourceHandler::GetMaterial(MaterialHandle handle)
	{
		return GetHandler()->m_materials.GetResource(handle);
	}

	ResourceHandler* ResourceHandler::GetHandler()
	{
		static ResourceHandler* handler = new ResourceHandler();
		return handler;
	}

}