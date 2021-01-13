#include "aepch.h"
#include "Texture.h"
#include "RenderAPI.h"
#include "AstralEngine/Platform/OpenGL/OpenGLTexture.h"

namespace AstralEngine 
{
	AReference<Texture2D> Texture2D::Create(unsigned int width, unsigned int height, void* data, unsigned int size)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:
			AE_CORE_ERROR("No RenderAPI is not yet supported");

		case RenderAPI::API::OpenGL:
			return AReference<OpenGLTexture2D>::Create(width, height, data, size);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}

	AReference<Texture2D> Texture2D::Create(unsigned int width, unsigned int height)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:
			AE_CORE_ERROR("No RenderAPI is not yet supported");

		case RenderAPI::API::OpenGL:
			return AReference<OpenGLTexture2D>::Create(width, height);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}

	AReference<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:
			AE_CORE_ERROR("No RenderAPI is not yet supported");

		case RenderAPI::API::OpenGL:
			return AReference<OpenGLTexture2D>::Create(path);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}

}