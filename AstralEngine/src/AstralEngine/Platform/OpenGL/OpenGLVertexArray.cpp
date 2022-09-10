#include "aepch.h"
#include "OpenGLVertexArray.h"
#include "AstralEngine/Renderer/VertexBuffer.h"
#include <glad/glad.h>

namespace AstralEngine
{
	OpenGLVertexArray* OpenGLVertexArray::s_currBoundVA = nullptr;


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
		if (s_currBoundVA == this)
		{
			Unbind();
			s_currBoundVA = nullptr;
		}
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_rendererID);
		s_currBoundVA = const_cast<OpenGLVertexArray*>(this);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
		s_currBoundVA = nullptr;
	}

	void OpenGLVertexArray::SetLayout(const VertexBufferLayout& layout, size_t layoutOffset)
	{
		AE_PROFILE_FUNCTION();
		int offset = 0;
		int indexOffset = 0;

		for (int i = 0; i < layout.GetCount(); i++)
		{
			/*
			glEnableVertexAttribArray(i + layoutOffset);
			glVertexAttribPointer(i + layoutOffset, layout[i].GetComponentCount(), 
				ADataTypeToOpenGLBaseType(layout[i].type),
				layout[i].normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(), (const void*)offset);
			offset += layout[i].size;
			*/

			size_t pos = i + indexOffset + layoutOffset;
			if (layout[i].type == ADataType::Mat3)
			{
				constexpr size_t sizeVec3 = sizeof(Vector3);
				glEnableVertexAttribArray(pos);
				glVertexAttribPointer(pos, 3, GL_FLOAT, layout[i].normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(), (const void*)offset);
				glVertexAttribDivisor(pos, layout[i].advanceRate);

				pos++;
				glEnableVertexAttribArray(pos);
				glVertexAttribPointer(pos, 3, GL_FLOAT, layout[i].normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(), (const void*)(offset + sizeVec3));
				glVertexAttribDivisor(pos, layout[i].advanceRate);

				pos++;
				glEnableVertexAttribArray(pos);
				glVertexAttribPointer(pos, 3, GL_FLOAT, layout[i].normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(), (const void*)(offset + 2 * sizeVec3));
				glVertexAttribDivisor(pos, layout[i].advanceRate);

				offset += layout[i].size;
				indexOffset += 2;
			}
			else if (layout[i].type == ADataType::Mat4)
			{
				constexpr size_t sizeVec4 = sizeof(Vector4);
				glEnableVertexAttribArray(pos);
				glVertexAttribPointer(pos, 4, GL_FLOAT, layout[i].normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(), (const void*)offset);
				glVertexAttribDivisor(pos, layout[i].advanceRate);

				pos++;
				glEnableVertexAttribArray(pos);
				glVertexAttribPointer(pos, 4, GL_FLOAT, layout[i].normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(), (const void*)(offset + sizeVec4));
				glVertexAttribDivisor(pos, layout[i].advanceRate);

				pos++;
				glEnableVertexAttribArray(pos);
				glVertexAttribPointer(pos, 4, GL_FLOAT, layout[i].normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(), (const void*)(offset + 2 * sizeVec4));
				glVertexAttribDivisor(pos, layout[i].advanceRate);
				
				pos++;
				glEnableVertexAttribArray(pos);
				glVertexAttribPointer(pos, 4, GL_FLOAT, layout[i].normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(), (const void*)(offset + 3 * sizeVec4));
				glVertexAttribDivisor(pos, layout[i].advanceRate);

				offset += layout[i].size;
				indexOffset += 3;
			}
			else
			{
				glEnableVertexAttribArray(pos);
				glVertexAttribPointer(pos, layout[i].GetComponentCount(),
					ADataTypeToOpenGLBaseType(layout[i].type), layout[i].normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(), (const void*)(long long)offset);
				offset += layout[i].size;
				glVertexAttribDivisor(pos, layout[i].advanceRate);
			}
		}
	}

	OpenGLVertexArray* OpenGLVertexArray::GetCurrBoundVA()
	{
		return s_currBoundVA;
	}
}