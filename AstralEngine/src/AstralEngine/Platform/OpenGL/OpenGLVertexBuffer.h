#pragma once
#include "AstralEngine/Renderer/VertexBuffer.h"
#include "OpenGLVertexArray.h"

namespace AstralEngine
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(unsigned int size, bool isInstanceArr = false);
		OpenGLVertexBuffer(float* data, unsigned int dataSize, bool isInstanceArr = false);
		~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, unsigned int size, unsigned int offset = 0) override;
		virtual void SetLayout(const VertexBufferLayout& layout) override;
		virtual void SetLayout(const VertexBufferLayout& layout, size_t layoutOffset, size_t dataOffset) override;

	private:
		OpenGLVertexArray* m_vertexArray;
		unsigned int m_rendererID;
	};
}