#pragma once
#include "AstralEngine/Data Struct/AReference.h"

namespace AstralEngine
{
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() { }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(unsigned int* data, unsigned int count) = 0;

		virtual int GetCount() const = 0;

		static AReference<IndexBuffer> Create();
		static AReference<IndexBuffer> Create(unsigned int* indices, unsigned int count);
	};
}