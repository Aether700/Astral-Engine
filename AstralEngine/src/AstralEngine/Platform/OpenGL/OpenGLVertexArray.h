#pragma once
#include "AstralEngine/Renderer/VertexArray.h"

namespace AstralEngine
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetLayout(const VertexBufferLayout& layout) override;

	private:
		unsigned int m_rendererID;
	};
}