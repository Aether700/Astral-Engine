#pragma once
#include "ADoublyLinkedList.h"

namespace AstralEngine
{
	template<typename T>
	class APriorityQueue
	{
	public:
		APriorityQueue() { }

		~APriorityQueue() { }

		//negative priority goes first then positive => (-1, 0, 1) =>
		void Enqueue(T& element, int priority = 0) //
		{
			QueueNode newNode = QueueNode(element , priority);
			int i = 0;
			for (QueueNode n : m_list)
			{
				if (newNode.priority >= n.priority)
				{
					break;
				}
				i++;
			}

			m_list.Insert(newNode, i);
		}

		T& Dequeue()
		{
			

			AE_DATASTRUCT_ASSERT(!m_list.IsEmpty(), "Cannot Dequeue an Empty APriorityQueue");
			T element = std::move(m_list[m_list.GetCount() - 1]);
			m_list.Remove(m_list.GetCount() - 1);

			return element;
		}

		const T& Peek() const
		{
			return m_list[m_list.GetCount() - 1].element;
		}

		void Clear() { m_list.Clear(); }

		bool IsEmpty() const { return m_list.IsEmpty(); }

	private:

		struct QueueNode
		{
			T element;
			int priority;

			QueueNode() { }
			QueueNode(T e, int p) : element(e), priority(p) { }

			bool operator==(const QueueNode& other)
			{
				return this == &other;
			}

		};

		ADoublyLinkedList<QueueNode> m_list;
	};
}