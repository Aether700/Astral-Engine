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
		virtual void SetLayout(const VertexBufferLayout& layout, size_t layoutOffset, size_t dataOffset) override;

		static OpenGLVertexArray* GetCurrBoundVA();

	private:
		static OpenGLVertexArray* s_currBoundVA;

		unsigned int m_rendererID;
	};
}