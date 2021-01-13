#pragma once
#include "ADoublyLinkedList.h"

namespace AstralEngine
{
	template<typename T>
	class AQueue
	{
	public:
		AQueue() { }

		~AQueue() { }

		void Enqueue(T& element) //
		{
			AE_PROFILE_FUNCTION();
			m_list.AddFirst(element);
		}

		T& Dequeue()
		{
			AE_PROFILE_FUNCTION();

			AE_CORE_ASSERT(!m_list.IsEmpty(), "Cannot Dequeue an Empty AQueue");
			T element = std::move(m_list[m_list.GetCount() - 1]);
			m_list.Remove(m_list.GetCount() - 1);

			return element;
		}

		const T& Peek() const
		{
			return m_list[m_list.GetCount() - 1];
		}

		void Clear() { m_list.Clear(); }

		bool IsEmpty() const { return m_list.IsEmpty(); }

	private:
		ADoublyLinkedList<T> m_list;

	};
}