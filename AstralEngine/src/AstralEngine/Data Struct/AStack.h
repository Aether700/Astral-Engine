#pragma once
#include "ASinglyLinkedList.h"

namespace AstralEngine
{
	template<typename T>
	class AStack
	{
	public:
		AStack() { }
		~AStack() { }

		void Push(T& element) { m_list.Add(element); }
		
		T& Pop() 
		{
			AE_ASSERT(!m_list.IsEmpty());
			T& popped = m_list[0];
			m_list.Remove(0);
			return popped;
		}

		T& Peek() const
		{
			AE_ASSERT(!m_list.IsEmpty());
			return m_list[0];
		}

		size_t GetCount() const { return m_list.GetCount(); }

		bool IsEmpty() const { return m_list.IsEmpty(); }

		size_t Find(const T& element) const { return m_list.Find(element); }

	private:
		ASinglyLinkedList<T> m_list;
	};
}