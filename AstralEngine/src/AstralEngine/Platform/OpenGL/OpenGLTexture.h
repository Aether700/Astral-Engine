#pragma once
#include "AstralEngine/Renderer/Texture.h"

namespace AstralEngine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(unsigned int width, unsigned int height);
		OpenGLTexture2D(unsigned int width, unsigned int height, void* data, unsigned int size);
		OpenGLTexture2D(const std::string& path);
		~OpenGLTexture2D();

		virtual unsigned int GetWidth() const override;
		virtual unsigned int GetHeight() const override;
		virtual unsigned int GetTextureID() const override;

		virtual void SetData(void* data, unsigned int size) override;

		virtual void Bind(unsigned int slot = 0) const override;

		virtual bool operator==(const Texture& other) const override;

	private:
		unsigned int m_rendererID;
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_internalFormat;
		unsigned int m_dataFormat;
	};
}