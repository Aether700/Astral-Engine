#include "aepch.h"
#include "OpenGLIndexBuffer.h"

#include <glad/glad.h>

namespace AstralEngine
{
	OpenGLIndexBuffer::OpenGLIndexBuffer() : m_count(0)
	{
		glCreateBuffers(1, &m_rendererID);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indices, unsigned int count) : m_count(count)
	{
		glCreateBuffers(1, &m_rendererID);
		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer() 
	{
		glDeleteBuffers(1, &m_rendererID);
	}

	void OpenGLIndexBuffer::Bind() const 
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	}

	void OpenGLIndexBuffer::Unbind() const 
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::SetData(const unsigned int* data, unsigned int count)
	{
		m_count = count;
		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(unsigned int), data, GL_DYNAMIC_DRAW);
	}
}