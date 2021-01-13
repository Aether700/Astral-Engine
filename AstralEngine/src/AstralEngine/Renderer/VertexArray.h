#pragma once
#include "AstralEngine/Data Struct/AReference.h"
#include "VertexBuffer.h"

namespace AstralEngine
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() { }
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const VertexBufferLayout& layout) = 0;

		static AReference<VertexArray> Create();
	};
}