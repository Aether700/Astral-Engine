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


	//CubeMap /////////////////////////////////////////

	AReference<CubeMap> CubeMap::Create(const std::array<std::string, 6>& faceTextures)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:
			AE_CORE_ERROR("No RenderAPI is not yet supported");

		case RenderAPI::API::OpenGL:
			return AReference<OpenGLCubeMap>::Create(faceTextures);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}

	//will apply the same texture to every face
	AReference<CubeMap> CubeMap::Create(const std::string& faceTexture)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:
			AE_CORE_ERROR("No RenderAPI is not yet supported");

		case RenderAPI::API::OpenGL:
			return AReference<OpenGLTexture2D>::Create(faceTexture);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}

	//applies the texture data provided to all sides of the cube
	AReference<CubeMap> CubeMap::Create(unsigned int size, void* data)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:
			AE_CORE_ERROR("No RenderAPI is not yet supported");

		case RenderAPI::API::OpenGL:
			return AReference<OpenGLCubeMap>::Create(size, data);
		}

		AE_CORE_ERROR("Unknown RenderAPI");
		return nullptr;
	}
}