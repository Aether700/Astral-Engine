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

		virtual bool IsSwapChainTarget() const override;

		virtual unsigned int GetColorAttachment() const override;
		virtual void Resize(unsigned int width, unsigned int height) override;

	private:
		unsigned int m_rendererID;
		unsigned int m_width, m_height;
		bool m_isSwapChainTarget;

		unsigned int m_colorAttachment, m_depthAttachment;
	};
}