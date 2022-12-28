#pragma once
#include "ADynArr.h"
#include "AStack.h"


namespace AstralEngine
{
	static constexpr size_t NullID = MAXSIZE_T;

	/*
	 generic class which provides a compact way to store data and access it using a size_t as key.
	 This data structure is slower than a HashMap/UnorderedMap and resembles more a table with an id/handle
	 system from a database
	*/
	template<typename T>
	class ADataStorage
	{
	public:
		size_t Add(const T& data)
		{
			size_t id;
			if (m_idToReuse.IsEmpty())
			{
				id = m_data.GetCount();
				m_data.Add(data);
			}
			else
			{
				id = m_idToReuse.Pop();
				m_data[handle] = data;
			}
			return id;
		}

		void Remove(size_t id)
		{
			AE_CORE_ASSERT(IDIsValid(id), "");
			m_idToReuse.Push(id);
		}

		T& Get(size_t id)
		{
			AE_CORE_ASSERT(IDIsValid(id), "Invalid id provided");
			return m_data[id];
		}

		const T& Get(size_t id) const
		{
			AE_CORE_ASSERT(IDIsValid(id), "Invalid id provided");
			return m_data[id];
		}

		size_t FindID(const T& data) const
		{
			int index = m_data.Find(data);
			if (index == -1 || m_idToReuse.Contains((size_t)index))
			{
				return NullID;
			}
			return (size_t)index;
		}

		bool Contains(const T& data) const
		{
			return FindID(data) != NullID;
		}

		bool IDIsValid(size_t id)
		{
			return handle < m_data.GetCount() && !m_idToReuse.Contains(id);
		}

		T& operator[](size_t id) { return Get(id); }
		const T& operator[](size_t id) const { return Get(id); }

	private:
		ADynArr<T> m_data;
		AStack<size_t> m_idToReuse;
	};
}