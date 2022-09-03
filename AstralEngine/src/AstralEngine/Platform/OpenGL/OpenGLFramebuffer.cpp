#include "aepch.h"
#include "OpenGLFramebuffer.h"
#include "AstralEngine/Renderer/Texture.h"

#include <glad/glad.h>

// temp
#include "AstralEngine/Core/Application.h"
///////

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

		Initialize(width, height);
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

	unsigned int OpenGLFramebuffer::GetRendererID() const
	{
		return m_rendererID;
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
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_2D,
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
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		// unbind all previous color attachments
		for (size_t i = 0; i < m_largestColorAttachmentIndex; i++)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// delete old framebuffer
		glDeleteFramebuffers(1, &m_rendererID);

		// create new framebuffer
		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		// resize all color attachments
		for (size_t i = 0; i < m_largestColorAttachmentIndex; i++)
		{
			if (m_colorAttachments[i] != NullHandle)
			{
				ResourceHandler::DeleteTexture2D(m_colorAttachments[i]);
				SetColorAttachment(ResourceHandler::CreateTexture2D(width, height), i);
			}
		}

		// resize depth attachment
		ResourceHandler::DeleteTexture2D(m_depthAttachment);
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

	void OpenGLFramebuffer::CopyTo(AReference<Framebuffer> targetFB) const
	{
		unsigned int targetID = 0;
		if (targetFB != nullptr)
		{
			targetID = targetFB->GetRendererID();
		}

		AE_CORE_ASSERT((targetFB == nullptr || (targetFB->GetHeight() == m_height 
			&& targetFB->GetWidth() == m_width)), "Trying to copy data between incompatible framebuffers");

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rendererID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetID); 
		glBlitFramebuffer(0, 0, GetWidth(), GetHeight(), 0, 0, 
			GetWidth(), GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void OpenGLFramebuffer::Initialize(unsigned int width, unsigned int height)
	{
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