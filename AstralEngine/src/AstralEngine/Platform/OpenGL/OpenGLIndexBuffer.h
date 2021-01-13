#pragma once
#include "AstralEngine/Renderer/IndexBuffer.h"

namespace AstralEngine
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(unsigned int* indices, unsigned int count);
		~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		inline virtual int GetCount() const override {	return m_count;	}

	private:
		unsigned int m_rendererID;
		unsigned int m_count;
	};
}