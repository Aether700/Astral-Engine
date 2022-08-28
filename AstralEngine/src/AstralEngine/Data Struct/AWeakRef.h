#pragma once
#include "AReference.h"

namespace AstralEngine
{
	template<typename T>
	class AWeakRef
	{
		template<typename T>
		friend class AReference;

	public:
		AWeakRef()
		{
			m_block = nullptr;
		}

		AWeakRef(std::nullptr_t null)
		{
			m_block = nullptr;
		}

		AWeakRef(const AWeakRef<T>& other) : m_block(other.m_block) 
		{
			if (m_block != nullptr)
			{
				m_block->weakCount++;
			}
		}
		
		AWeakRef(const AReference<T>& other) : m_block(other.m_block)
		{
			if (m_block != nullptr)
			{
				m_block->weakCount++;
			}
		}

		~AWeakRef()
		{
			if (m_block != nullptr)
			{
				m_block->DecrementWeak();
			}
		}

		T* operator->() const
		{
			return (T*)m_block->ptr;
		}

		T& operator*() const
		{
			return *((T*)m_block->ptr);
		}

		void operator=(const AWeakRef<T>& other)
		{
			if (m_block != nullptr)
			{
				m_block->DecrementWeak();
			}
			m_block = other.m_block;

			if (m_block != nullptr)
			{
				m_block->weakCount++;
			}
		}

		void operator=(const AReference<T>& other)
		{
			if (m_block != nullptr)
			{
				m_block->DecrementWeak();
			}
			m_block = other.m_block;
			
			if (m_block != nullptr)
			{
				m_block->weakCount++;
			}
		}
		
		bool operator==(const AWeakRef<T>& other) const
		{
			if (m_block == nullptr)
			{
				return other.m_block == nullptr;
			}

			return *((T*)m_block->ptr) == *((T*)other.m_block->ptr);
		}

		bool operator==(const AReference<T>& other) const
		{
			if (m_block == nullptr)
			{
				return other.m_block == nullptr;
			}

			return *((T*)m_block->ptr) == *((T*)other.m_block->ptr);
		}

		bool operator!=(const AWeakRef<T>& other) const
		{
			
			return !(*this == other);
		}
		
		bool operator!=(const AReference<T>& other) const
		{
			
			return !(*this == other);
		}

		bool IsExpired() const 
		{ 
			if (m_block == nullptr)
			{
				return false;
			}
			return m_block->ptr == nullptr;
		}

	private:
		ControlBlock* m_block;
	};
}