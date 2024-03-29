#include "aepch.h"
#include "OpenGLVertexBuffer.h"

#include <glad/glad.h>

namespace AstralEngine
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(unsigned int size, bool isInstanceArr)
	{
		if (!isInstanceArr)
		{
			m_vertexArray = new OpenGLVertexArray();
			m_vertexArray->Bind();
		}
		else
		{
			m_vertexArray = nullptr;
		}
		glCreateBuffers(1, &m_rendererID);

		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* data, unsigned int dataSize, bool isInstanceArr)
	{
		if (!isInstanceArr)
		{
			m_vertexArray = new OpenGLVertexArray();
			m_vertexArray->Bind();
		}
		else
		{
			m_vertexArray = nullptr;
		}
		glCreateBuffers(1, &m_rendererID);

		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
	}
	
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		delete m_vertexArray;
		glDeleteBuffers(1, &m_rendererID);
	}

	void OpenGLVertexBuffer::Bind() const 
	{
		if (m_vertexArray != nullptr)
		{
			m_vertexArray->Bind();
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	}

	void OpenGLVertexBuffer::Unbind() const 
	{
		if (m_vertexArray != nullptr)
		{
			m_vertexArray->Unbind();
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, unsigned int size, unsigned int offset)
	{
		AE_PROFILE_FUNCTION();
		Bind();
		if (offset > 0)
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		}
		else
		{
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
		}
	}

	void OpenGLVertexBuffer::SetLayout(const VertexBufferLayout& layout, size_t layoutOffset)
	{
		if (m_vertexArray != nullptr)
		{
			m_vertexArray->SetLayout(layout, layoutOffset);
		}
		else
		{
			OpenGLVertexArray* boundVA = OpenGLVertexArray::GetCurrBoundVA();
			if (boundVA != nullptr);
			{
				boundVA->SetLayout(layout, layoutOffset);
			}
		}

		/*
		int indexOffset = 0;
		for (size_t i = 0; i < layout.GetCount(); i++)
		{
			if (layout[i].type == ADataType::Mat3)
			{
				glVertexAttribDivisor(i + layoutOffset + indexOffset, layout[i].advanceRate);
				glVertexAttribDivisor(i + layoutOffset + indexOffset + 1, layout[i].advanceRate);
				glVertexAttribDivisor(i + layoutOffset + indexOffset + 2, layout[i].advanceRate);
				indexOffset += 2;
			}
			else if (layout[i].type == ADataType::Mat4)
			{
				glVertexAttribDivisor(i + layoutOffset + indexOffset, layout[i].advanceRate);
				glVertexAttribDivisor(i + layoutOffset + indexOffset + 1, layout[i].advanceRate);
				glVertexAttribDivisor(i + layoutOffset + indexOffset + 2, layout[i].advanceRate);
				glVertexAttribDivisor(i + layoutOffset + indexOffset + 3, layout[i].advanceRate);
				indexOffset += 3;
			}
			else
			{
				glVertexAttribDivisor(i + layoutOffset + indexOffset, layout[i].advanceRate);
			}
		}
		*/
	}
}