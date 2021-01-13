#pragma once
#include <utility>
#include "AKeyElementPair.h"
#include "ADynArr.h"
#include "ADelegate.h"

namespace AstralEngine
{
	//default compare function which results in a min heap
	template<typename K>
	static bool DefaultCompare(const K& lhs, const K& rhs)
	{
		return lhs < rhs;
	}

	//by default the heap is a min heap but by passing the correct compare function it can also be used as a max heap;
	template<typename K, typename T>
	class AHeap
	{
		using CompareFunc = ADelegate<bool(const K&, const K&)>;
	public:

		using AIterator = typename ADynArr<AKeyElementPair<K, T>>::AIterator;
		using AConstIterator = typename ADynArr<AKeyElementPair<K, T>>::AConstIterator;

		AHeap() : m_compareFunc(FunctionWraper<&DefaultCompare<K>>{}) { }

		AHeap(size_t initialCapacity) : m_compareFunc(FunctionWraper<&DefaultCompare<K>>{}),
			m_arr(initialCapacity) { }

		AHeap(CompareFunc& func) : m_compareFunc(func) { }

		AHeap(CompareFunc& func, size_t initialCapacity) : m_compareFunc(func),
			m_arr(initialCapacity) { }

		void Enqueue(const K& key, const T& element)
		{
			AE_PROFILE_FUNCTION();
			AKeyElementPair<K, T> pair = AKeyElementPair<K, T>(key, element);
			Enqueue(pair);
		}

		void Enqueue(const AKeyElementPair<K, T>& pair)
		{
			AE_PROFILE_FUNCTION();
			m_arr.Add(pair);
			UpHeap();
		}

		AKeyElementPair<K, T> Dequeue()
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(!IsEmpty(), "Cannot dequeue an empty heap");
			AKeyElementPair<K, T> pair = m_arr[0];

			//swap then remove to increase performance
			Swap(0, m_arr.GetCount() - 1);
			m_arr.Remove(m_arr.GetCount() - 1);
			
			DownHeap();
			return pair;
		}

		size_t GetCount() const { return m_arr.GetCount(); }

		bool IsEmpty() const { return m_arr.IsEmpty(); }

		void Clear() { m_arr.Clear(); }

		//begin and end iterator functions
		AIterator begin() { return m_arr.begin(); }
		AIterator end() { return m_arr.end(); }
		AConstIterator begin() const { return m_arr.begin(); }
		AConstIterator end() const { return m_arr.end(); }

		bool operator==(const AHeap& other) const
		{
			return m_arr == other.m_arr && m_compareFunc == other.m_compareFunc;
		}

		bool operator!=(const AHeap& other) const
		{
			return !(*this == other);
		}


	private:

		size_t GetParent(size_t index) const
		{
			if (index % 2 == 0)
			{
				return (index / 2) - 1;
			}
			return index / 2;
		}

		size_t GetLeft(size_t index) const
		{
			return ((index + 1) * 2) - 1;
		}

		size_t GetRight(size_t index) const
		{
			return (index + 1) * 2;
		}

		void Swap(size_t lhs, size_t rhs)
		{
			if (lhs == rhs)
			{
				return;
			}

			AKeyElementPair<K, T> temp = std::move(m_arr[lhs]);
			m_arr[lhs] = std::move(m_arr[rhs]);
			m_arr[rhs] = std::move(temp);
		}

		bool HasLeft(size_t index) const
		{
			return GetLeft(index) < m_arr.GetCount();
		}

		bool HasRight(size_t index) const
		{
			return GetRight(index) < m_arr.GetCount();
		}

		void UpHeap()
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(!IsEmpty(), "Cannot perform up heap an empty heap");
			
			size_t last = m_arr.GetCount() - 1;

			//if root exit
			if (last == 0)
			{
				return;
			}

			while (last != 0 && m_compareFunc(m_arr[last].GetKey(), m_arr[GetParent(last)].GetKey()))
			{
				size_t parent = GetParent(last);
				Swap(last, parent);
				last = parent;
			}
		}

		void DownHeap()
		{
			AE_PROFILE_FUNCTION();
			if (IsEmpty())
			{
				return;
			}

			size_t curr = 0;
			size_t next = curr;

			//make sure has a child (if no left then no right either)
			while (HasLeft(curr))
			{
				if (m_compareFunc(m_arr[GetLeft(curr)].GetKey(), m_arr[curr].GetKey()))
				{
					next = GetLeft(curr);
				}

				if (GetRight(curr) < m_arr.GetCount())
				{
					if (m_compareFunc(m_arr[GetRight(curr)].GetKey(), m_arr[next].GetKey()))
					{
						next = GetRight(curr);
					}
				}

				if (curr == next)
				{
					break;
				}

				Swap(curr, next);
				curr = next;
			}
		}

		ADynArr<AKeyElementPair<K, T>> m_arr;
		CompareFunc m_compareFunc;
	};

}