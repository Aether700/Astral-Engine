#include "aepch.h"
#include "OpenGLVertexArray.h"
#include "AstralEngine/Renderer/VertexBuffer.h"
#include <glad/glad.h>

namespace AstralEngine
{

	// temp function will change place later
	static GLenum ADataTypeToOpenGLBaseType(ADataType type)
	{
		switch (type)
		{
		case ADataType::Float:
		case ADataType::Float2:
		case ADataType::Float3:
		case ADataType::Float4:
		case ADataType::Mat3:
		case ADataType::Mat4:
			return GL_FLOAT;

		case ADataType::Int:
		case ADataType::Int2:
		case ADataType::Int3:
		case ADataType::Int4:
			return GL_INT;

		case ADataType::Bool:
			return GL_BOOL;
		}

		AE_CORE_ASSERT(false, "Unknown Shader data type");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_rendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_rendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::SetLayout(const VertexBufferLayout& layout, size_t layoutOffset)
	{
		AE_PROFILE_FUNCTION();
		int offset = 0;

		for (int i = layoutOffset; i < layout.GetCount(); i++)
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, layout[i].GetComponentCount(), ADataTypeToOpenGLBaseType(layout[i].type),
				layout[i].normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(), (const void*)(long long) offset);
			offset += layout[i].size;
		}
	}
}