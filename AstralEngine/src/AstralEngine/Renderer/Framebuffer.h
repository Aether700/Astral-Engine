#pragma once
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Math/AMath.h"

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

		virtual bool IsSwapChainTarget() const = 0;

		virtual unsigned int GetColorAttachment() const = 0;
		virtual void Resize(unsigned int width, unsigned int height) = 0;

		static AReference<Framebuffer> Create(unsigned int width, unsigned int height, bool isSwapChainTarget = false);

	private:

	};
}