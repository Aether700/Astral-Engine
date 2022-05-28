#include "aepch.h"
#include "OpenGLTexture.h"
#include <stb_image.h>
#include <glad/glad.h>

namespace AstralEngine
{
	//OpenGLTexture2D///////////////////////////////////////////////////////////////////
	OpenGLTexture2D::OpenGLTexture2D(unsigned int width, unsigned int height) 
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

	//OpenGLCubeMap//////////////////////////////////////////////////////////////////////

	/*pass the texture used for each face where each texture
	  is applied in this order
	
	  index 0 -> right face (positive X)
	  index 1 -> left face (negative X)
	  index 2 -> top face (positive Y)
	  index 3 -> bottom face (negative Y)
	  index 4 -> front face (positive Z)
	  index 5 -> back face (negative Z)
	*/
	OpenGLCubeMap::OpenGLCubeMap(const std::array<std::string, 6>& faceTextures) : m_size(0)
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_rendererID);
		Bind();

		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		unsigned char* data;

		for (size_t i = 0; i < 6; i++)
		{
			data = stbi_load(faceTextures[i].c_str(), &width, &height, &channels, 0);

			if (data == nullptr)
			{
				AE_CORE_ERROR("Failed to load image \"%s\"", faceTextures[i]);
			}

			unsigned int size = (unsigned int)Math::Min(width, height);


			if ((m_size != 0) && (size != m_size))
			{
				AE_CORE_ERROR("all textures passed to the cube map must have the same size");
			}
			else if (m_size == 0)
			{
				m_size = size;
			}

			switch (channels)
			{
			case 3:
				m_internalFormat[i] = GL_RGB8;
				m_dataFormat[i] = GL_RGB;
				break;

			case 4:
				m_internalFormat[i] = GL_RGBA8;
				m_dataFormat[i] = GL_RGBA;
				break;

			default:
				AE_CORE_ERROR("The Format provided for the texture '%S' is not supported", faceTextures[i]);
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat[i], m_size, m_size,
				0, m_dataFormat[i], GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}

	//will apply the same texture to every face
	OpenGLCubeMap::OpenGLCubeMap(const std::string& faceTexture)
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_rendererID);
		Bind();
		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;

		unsigned char* data = stbi_load(faceTexture.c_str(), &width, &height, &channels, 0);

		if (data == nullptr)
		{
			AE_CORE_ERROR("Failed to load image from file %S", faceTexture);
		}

		m_size = Math::Min(width, height);

		switch (channels)
		{
		case 3:
			m_internalFormat[0] = GL_RGB8;
			m_dataFormat[0] = GL_RGB;
			break;

		case 4:
			m_internalFormat[0] = GL_RGBA8;
			m_dataFormat[0] = GL_RGBA;
			break;

		default:
			AE_CORE_ERROR("The Format provided for the texture '%S' is not supported", faceTexture);
		}

		for (size_t i = 0; i < 6; i++)
		{
			m_internalFormat[i] = m_internalFormat[0];
			m_dataFormat[i] = m_dataFormat[0];

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat[i], m_size, m_size,
				0, m_dataFormat[i], GL_UNSIGNED_BYTE, data);
		}

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_R, GL_REPEAT);

		stbi_image_free(data);
	}

	//applies the texture data provided to all sides of the cube
	OpenGLCubeMap::OpenGLCubeMap(unsigned int size, void* data)
		: m_size(size)
	{
		AE_CORE_ASSERT(data != nullptr && size > 0, "incorrect data/size passed to OpenGLCubeMap constructor");

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_rendererID);
		m_internalFormat[0] = GL_RGB8;
		m_dataFormat[0] = GL_RGB;

		Bind();

		for (size_t i = 0; i < 6; i++)
		{
			m_internalFormat[i] = m_internalFormat[0];
			m_dataFormat[i] = m_dataFormat[0];

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat[i], m_size, m_size,
				0, m_dataFormat[i], GL_UNSIGNED_BYTE, data);
		}


		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_R, GL_REPEAT);


	}

	OpenGLCubeMap::~OpenGLCubeMap()
	{
		if (m_size != 0)
		{
			glDeleteTextures(1, &m_rendererID);
		}
	}

	void OpenGLCubeMap::SetData(void* data, unsigned int size)
	{
		AE_PROFILE_FUNCTION();
		unsigned int bytesPerChannel = 0;

		switch (m_dataFormat[0])
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

		AE_CORE_ASSERT(size == m_size * m_size * bytesPerChannel, "Data must be entire texture");
		
		for (size_t i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat[i], m_size, m_size,
				0, m_dataFormat[i], GL_UNSIGNED_BYTE, data);
		}
	}

	//binds the cubemap to the provided texture slot (defaults to slot 0)
	void OpenGLCubeMap::Bind(unsigned int slot) const
	{
		glBindTextureUnit(slot, m_rendererID);
	}
}