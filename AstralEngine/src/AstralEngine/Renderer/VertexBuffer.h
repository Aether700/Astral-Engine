#pragma once
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Data Struct/ADynArr.h"

namespace AstralEngine
{
	enum class ADataType
	{
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	static unsigned int ADataTypeSize(ADataType type)
	{
		switch (type)
		{
		case ADataType::Float:		return sizeof(float);
		case ADataType::Float2:	return 2 * sizeof(float);
		case ADataType::Float3:	return 3 * sizeof(float);
		case ADataType::Float4:	return 4 * sizeof(float);
		case ADataType::Mat3:		return 3 * 3 * sizeof(float); // 3x3 matrix
		case ADataType::Mat4:		return 4 * 4 * sizeof(float); //4x4 matrix
		case ADataType::Int:		return sizeof(int);
		case ADataType::Int2:		return 2 * sizeof(int);
		case ADataType::Int3:		return 3 * sizeof(int);
		case ADataType::Int4:		return 4 * sizeof(int);
		case ADataType::Bool:		return sizeof(bool);
		}
	}

	struct LayoutElement
	{
		std::string name;
		ADataType type;
		unsigned int size;
		bool normalized;

		LayoutElement() { }

		LayoutElement(ADataType t, const std::string& n, bool nor = false) : type(t), name(n), 
			normalized(nor), size(ADataTypeSize(t)) { }

		unsigned int GetComponentCount() const
		{
			switch (type)
			{
				case ADataType::Float:		return 1;
				case ADataType::Float2:		return 2;
				case ADataType::Float3:		return 3;
				case ADataType::Float4:		return 4;
				case ADataType::Mat3:		return 3 * 3; // 3x3 matrix
				case ADataType::Mat4:		return 4 * 4; //4x4 matrix
				case ADataType::Int:		return 1;
				case ADataType::Int2:		return 2;
				case ADataType::Int3:		return 3;
				case ADataType::Int4:		return 4;
				case ADataType::Bool:		return 1;
			}

			AE_CORE_ASSERT(false, "Unknown data type");
			return 0;
		}

		bool operator==(const LayoutElement& other) const
		{
			return name == other.name && type == other.type && normalized == other.normalized;
		}
	};

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout(const std::initializer_list<LayoutElement>& elements) : m_elements(elements)
		{
			CalculateStride();
		}

		~VertexBufferLayout() { }

		unsigned int GetStride() const { return m_stride; }
		
		inline size_t GetCount() const { return m_elements.GetCount(); }

		const LayoutElement& operator[](unsigned int index) const
		{
			return m_elements[index];
		}

	private:
		void CalculateStride()
		{
			m_stride = 0;
			for (LayoutElement& element : m_elements)
			{
				m_stride += element.size;
			}
		}

		ADynArr<LayoutElement> m_elements;
		unsigned int m_stride;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() { }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, unsigned int size, unsigned int offset = 0) = 0;
		virtual void SetLayout(const VertexBufferLayout& layout) = 0;

		static AReference<VertexBuffer> Create(unsigned int size);
		static AReference<VertexBuffer> Create(float* data, unsigned int dataSize);
	};
}