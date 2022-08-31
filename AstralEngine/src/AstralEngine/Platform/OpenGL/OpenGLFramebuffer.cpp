#include "aepch.h"
#include "OpenGLFramebuffer.h"
#include "AstralEngine/Renderer/Texture.h"

#include <glad/glad.h>

namespace AstralEngine
{
	OpenGLFramebuffer::OpenGLFramebuffer(unsigned int width, unsigned int height, bool isSwapChainTarget) 
		: m_rendererID(0), m_isSwapChainTarget(isSwapChainTarget)
	{
		AE_CORE_ASSERT((width > 0 && height > 0), "Invalid size provided for framebuffer initialization");
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

	Texture2DHandle OpenGLFramebuffer::GetColorAttachment() const
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
			ResourceHandler::DeleteTexture2D(m_colorAttachment);
			ResourceHandler::DeleteTexture2D(m_depthAttachment);
			glDeleteFramebuffers(1, &m_rendererID);
		}

		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		
		m_colorAttachment = ResourceHandler::CreateTexture2D(width, height);
		AReference<Texture2D> colorAttachment = ResourceHandler::GetTexture2D(m_colorAttachment);
		/*
		glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width,
			height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		*/

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
			colorAttachment->GetTextureID(), 0);

		m_depthAttachment = ResourceHandler::CreateTexture2D(width, height);
		AReference<Texture2D> depthAttachment = ResourceHandler::GetTexture2D(m_depthAttachment);

		/*
		glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);
		*/

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 
			depthAttachment->GetTextureID(), 0);

		need to fix depth buffer attachment

		AE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, 
			"Framebuffer is incomplete. Status: %d", glCheckFramebufferStatus(GL_FRAMEBUFFER));

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_width = width;
		m_height = height;
	}

}