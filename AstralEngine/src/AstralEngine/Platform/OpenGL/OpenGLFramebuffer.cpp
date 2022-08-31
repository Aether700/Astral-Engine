#include "aepch.h"
#include "OpenGLFramebuffer.h"
#include "AstralEngine/Renderer/Texture.h"

#include <glad/glad.h>

namespace AstralEngine
{
	size_t OpenGLFramebuffer::s_numColorAttachments = 0;

	OpenGLFramebuffer::OpenGLFramebuffer(unsigned int width, unsigned int height, bool isSwapChainTarget) 
		: m_rendererID(0), m_isSwapChainTarget(isSwapChainTarget)
	{
		AE_CORE_ASSERT((width > 0 && height > 0), "Invalid size provided for framebuffer initialization");
		if (s_numColorAttachments == 0)
		{
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (GLint*) &s_numColorAttachments);
		}

		m_colorAttachments = new Texture2DHandle[s_numColorAttachments];
		for(size_t i = 0; i < s_numColorAttachments; i++)
		{
			m_colorAttachments[i] = NullHandle;
		}

		m_largestColorAttachmentIndex = 1;

		Resize(width, height);
	}
	
	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_rendererID);
		delete[] m_colorAttachments;
	}

	void OpenGLFramebuffer::Bind() const 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		ADynArr<unsigned int> attachments = ADynArr<unsigned int>(m_largestColorAttachmentIndex);
		for (size_t i = 0; i < m_largestColorAttachmentIndex; i++)
		{
			if (m_colorAttachments[i] != NullHandle);
			{
				attachments.Add(GL_COLOR_ATTACHMENT0 + i);
			}
		}
		glDrawBuffers(attachments.GetCount(), attachments.GetData());
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

	Texture2DHandle OpenGLFramebuffer::GetColorAttachment(size_t attachmentIndex) const
	{

		return m_colorAttachments[attachmentIndex];
	}

	void OpenGLFramebuffer::SetColorAttachment(Texture2DHandle texture, size_t attachmentIndex)
	{
		AE_CORE_ASSERT(attachmentIndex < s_numColorAttachments, "");
		m_colorAttachments[attachmentIndex] = texture;
		AReference<Texture2D> colorAttachment = ResourceHandler::GetTexture2D(texture);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			colorAttachment->GetTextureID(), 0);
		if (m_largestColorAttachmentIndex < attachmentIndex + 1)
		{
			m_largestColorAttachmentIndex = attachmentIndex + 1;
		}
	}

	void OpenGLFramebuffer::Resize(unsigned int width, unsigned int height)
	{
		if (width < 1 || height < 1)
		{
			return;
		}

		if (m_rendererID != 0)
		{
			ResourceHandler::DeleteTexture2D(m_colorAttachments[0]);
			ResourceHandler::DeleteTexture2D(m_depthAttachment);
			glDeleteFramebuffers(1, &m_rendererID);
		}

		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		
		m_colorAttachments[0] = ResourceHandler::CreateTexture2D(width, height);
		SetColorAttachment(m_colorAttachments[0], 0);
		m_largestColorAttachmentIndex = 1;

		m_depthAttachment = ResourceHandler::CreateTexture2D(width, height, 
			Texture2DInternalFormat::Depth24Stencil8);
		AReference<Texture2D> depthAttachment = ResourceHandler::GetTexture2D(m_depthAttachment);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 
			depthAttachment->GetTextureID(), 0);

		AE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, 
			"Framebuffer is incomplete. Status: %d", glCheckFramebufferStatus(GL_FRAMEBUFFER));

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_width = width;
		m_height = height;
	}

}