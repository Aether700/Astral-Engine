#include "aepch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace AstralEngine
{
	OpenGLFramebuffer::OpenGLFramebuffer(unsigned int width, unsigned int height, bool isSwapChainTarget) 
		: m_rendererID(0), m_isSwapChainTarget(isSwapChainTarget)
	{
		AE_CORE_ASSERT(!(width < 1 || height < 1), "Invalid size provided for framebuffer initialization");
		Resize(width, height);
	}
	
	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_rendererID);
	}

	void OpenGLFramebuffer::Bind() const 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
	}

	void OpenGLFramebuffer::Unbind() const 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int OpenGLFramebuffer::GetWidth() const 
	{
		return m_width;
	}

	unsigned int OpenGLFramebuffer::GetHeight() const
	{
		return m_height;
	}

	bool OpenGLFramebuffer::IsSwapChainTarget() const
	{
		return m_isSwapChainTarget;
	}

	unsigned int OpenGLFramebuffer::GetColorAttachment() const
	{
		return m_colorAttachment;
	}

	void OpenGLFramebuffer::Resize(unsigned int width, unsigned int height)
	{
		if (width < 1 || height < 1)
		{
			return;
		}

		if (m_rendererID != 0)
		{
			glDeleteFramebuffers(1, &m_rendererID);
			glDeleteTextures(1, &m_colorAttachment);
			glDeleteTextures(1, &m_depthAttachment);
		}

		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width,
			height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);

		AE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_width = width;
		m_height = height;
	}

}