#pragma once
#include "AstralEngine/Renderer/Framebuffer.h"

namespace AstralEngine
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(unsigned int width, unsigned int height, bool isSwapChainTarget);
		~OpenGLFramebuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual unsigned int GetWidth() const override;
		virtual unsigned int GetHeight() const override;
		virtual unsigned int GetRendererID() const override;

		virtual bool IsSwapChainTarget() const override;

		virtual Texture2DHandle GetColorAttachment(size_t attachmentIndex = 0) const override;
		virtual void SetColorAttachment(Texture2DHandle texture, size_t attachmentIndex) override;
		virtual void Resize(unsigned int width, unsigned int height) override;

		virtual void CopyTo(AReference<Framebuffer> targetFB) const override;

	private:
		static size_t s_numColorAttachments;
		
		void Initialize(unsigned int width, unsigned int height);

		unsigned int m_rendererID;
		unsigned int m_width, m_height;
		bool m_isSwapChainTarget;

		Texture2DHandle* m_colorAttachments;
		size_t m_largestColorAttachmentIndex;

		Texture2DHandle m_depthAttachment;
	};
}