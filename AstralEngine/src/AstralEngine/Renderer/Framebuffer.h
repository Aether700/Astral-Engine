#pragma once
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Core/Resource.h"

namespace AstralEngine
{
	class Framebuffer
	{
	public:
		~Framebuffer() { }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual unsigned int GetRendererID() const = 0;


		virtual bool IsSwapChainTarget() const = 0;

		virtual Texture2DHandle GetColorAttachment(size_t attachmentIndex = 0) const = 0;
		virtual void SetColorAttachment(Texture2DHandle texture, size_t attachmentIndex) = 0;
		virtual void Resize(unsigned int width, unsigned int height) = 0;

		// copy the data from the current framebuffer to the one provided. If the provided framebuffer 
		// is nullptr the data of the framebuffer will be copied to the default framebuffer. 
		// Note that the current and target frame buffers need to have the same internal format 
		// for this function to work
		virtual void CopyTo(AReference<Framebuffer> targetFB) const = 0;

		static AReference<Framebuffer> Create(unsigned int width, unsigned int height, bool isSwapChainTarget = false);
	};
}