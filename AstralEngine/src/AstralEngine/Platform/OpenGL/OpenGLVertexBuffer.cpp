#include "aepch.h"
#include "OpenGLVertexBuffer.h"

#include <glad/glad.h>

namespace AstralEngine
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(unsigned int size)
	{
		m_vertexArray.Bind();
		glCreateBuffers(1, &m_rendererID);

		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* data, unsigned int dataSize)
	{
		m_vertexArray.Bind();
		glCreateBuffers(1, &m_rendererID);

		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
	}
	
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_rendererID);
	}

	void OpenGLVertexBuffer::Bind() const 
	{
		m_vertexArray.Bind();
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	}

	void OpenGLVertexBuffer::Unbind() const 
	{
		m_vertexArray.Unbind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, unsigned int size, unsigned int offset)
	{
		AE_PROFILE_FUNCTION();
		Bind();
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	}

	void OpenGLVertexBuffer::SetLayout(const VertexBufferLayout& layout, size_t layoutOffset)
	{
		m_vertexArray.SetLayout(layout, layoutOffset);
		for (size_t i = layoutOffset; i < layout.GetCount(); i++)
		{
			glVertexAttribDivisor(i, layout[i].advanceRate);
		}
	}
}