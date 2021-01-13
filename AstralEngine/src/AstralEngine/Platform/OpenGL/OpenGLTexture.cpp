#include "aepch.h"
#include "OpenGLTexture.h"
#include <stb_image.h>
#include <glad/glad.h>

namespace AstralEngine
{
	OpenGLTexture2D::OpenGLTexture2D(unsigned int width, unsigned height) 
		: m_internalFormat(GL_RGBA8), m_dataFormat(GL_RGBA), m_width(width), m_height(height) 
	{
		AE_PROFILE_FUNCTION();
		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(unsigned int width, unsigned int height, void* data, unsigned int size) 
		: m_internalFormat(GL_RGBA8), m_dataFormat(GL_RGBA), m_width(width), m_height(height)
	{
		AE_PROFILE_FUNCTION();
		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		SetData(data, size);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
	{
		AE_PROFILE_FUNCTION();
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		AE_CORE_ASSERT(data != nullptr, "failed to load texture '%S'", path);

		m_width = width;
		m_height = height;

		m_internalFormat = 0;

		switch(channels)
		{
		case 3:
			m_internalFormat = GL_RGB8;
			m_dataFormat = GL_RGB;
			break;

		case 4:
			m_internalFormat = GL_RGBA8;
			m_dataFormat = GL_RGBA;
			break;

		default:
			AE_CORE_ERROR("The Format provided for the texture '%S' is not supported", path);
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_rendererID);
	}

	unsigned int OpenGLTexture2D::GetWidth() const 
	{
		return m_width; 
	}

	unsigned int OpenGLTexture2D::GetHeight() const 
	{
		return m_height; 
	}

	unsigned int OpenGLTexture2D::GetTextureID() const
	{
		return m_rendererID;
	}

	void OpenGLTexture2D::SetData(void* data, unsigned int size)
	{
		AE_PROFILE_FUNCTION();
		unsigned int bytesPerChannel = 0;

		switch (m_dataFormat)
		{
		case GL_RGBA:
			bytesPerChannel = 4;
			break;

		case GL_RGB:
			bytesPerChannel = 3;
			break;

		default:
			AE_CORE_ERROR("Unknown Format");
		}

		AE_CORE_ASSERT(size == m_width * m_height * bytesPerChannel, "Data must be entire texture");
		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(unsigned int slot) const 
	{
		glBindTextureUnit(slot, m_rendererID);
	}

	bool OpenGLTexture2D::operator==(const Texture& other) const
	{
		return m_rendererID == ((OpenGLTexture2D&) other).m_rendererID;
	}
}