#pragma once
#include "AstralEngine/ECS/ECS Core/ECSUtils.h"
#include "ADynArr.h"
#include "AUniqueRef.h"
#include "ADelegate.h"
#include "AKeyElementPair.h"

#define AE_PAGE_SIZE 16384

namespace AstralEngine
{
	template<typename T>
	static size_t ToInt(const T x)
	{
		return (size_t) x;
	}

	template<typename...>
	class ASparseSet;

	template<typename T>
	class ASparseSet<T>
	{
		static constexpr size_t ElementPerPage = AE_PAGE_SIZE / sizeof(T);
		using PageType = typename AUniqueRef<T[]>;

		using ToIntFunc = ADelegate<size_t(const T)>;
		
	public:
		using AIterator = typename ADynArr<T>::AIterator;
		using AConstIterator = typename ADynArr<T>::AConstIterator;

		ASparseSet(ToIntFunc func = ToIntFunc(&ToInt<T>)) : m_toInt(func) { }

		virtual ~ASparseSet() { }

		bool Contains(const T& e) const
		{
			AE_PROFILE_FUNCTION();
			size_t page = Page(e);
			if (!(page < m_sparse.GetCount()) || m_sparse[page] == nullptr)
			{
				return false;
			}
			
			//use m_sparse[page][offset] to get actual index of e in the packed array
			size_t offset = Offset(e);
			return (size_t)m_sparse[page][offset] < m_packed.GetCount() 
				&& m_packed[(size_t)m_sparse[page][offset]] == e;
		}

		void Clear()
		{
			m_sparse.Clear();
			m_packed.Clear();
		}

		void ShrinkToFit()
		{
			m_packed.ShrinkToFit();
			m_sparse.ShrinkToFit();
		}

		size_t GetCount() const { return m_packed.GetCount(); }

		bool IsEmpty() const { return m_packed.IsEmpty(); }

		AIterator begin() { return m_packed.begin(); }

		AIterator end() { return m_packed.end(); }

		AConstIterator begin() const { return m_packed.begin(); }

		AConstIterator end() const { return m_packed.end(); }

		void Add(const T e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(!Contains(e), "Cannot add the same T twice");
			Assure(Page(e))[Offset(e)] = (T)m_packed.GetCount();
			m_packed.Add(e);
		}

		void Remove(const T e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Cannot remove an element that is not contained in the SparseSet");

			size_t page = Page(e);
			size_t offset = Offset(e);

			/*move last element to the position of the element to remove
			  so that removing the element is more efficient
			  (will also need to move the element in the sparse set)
			*/

			m_packed[m_toInt(m_sparse[page][offset])] = T(m_packed[m_packed.GetCount() - 1]);
			m_sparse[Page(m_packed[m_packed.GetCount() - 1])][Offset(m_packed[m_packed.GetCount() - 1])] = m_sparse[page][offset];
			m_sparse[page][offset] = Null;
			m_packed.RemoveAt(m_packed.GetCount() - 1);
		}

		size_t GetIndex(const T e) const
		{
			return (size_t)m_sparse[Page(e)][Offset(e)];
		}

		void Swap(const T lhs, const T rhs)
		{
			auto& left = m_sparse[Page(lhs)][Offset(lhs)];
			auto& right = m_sparse[Page(rhs)][Offset(rhs)];
			std::swap(m_packed[size_t(left)], m_packed[size_t(right)]);
			std::swap(left, right);
		}

		const T* GetData() const
		{
			return m_packed.GetData();
		}

	private:

		size_t Page(const T e) const
		{
			return size_t{ m_toInt(e) / ElementPerPage };
		}

		size_t Offset(const T e) const
		{
			return size_t{ m_toInt(e) & (ElementPerPage - 1) };
		}

		PageType& Assure(const size_t pos)
		{
			AE_PROFILE_FUNCTION();
			if (!(pos < m_sparse.GetCount()))
			{
				m_sparse.Reserve(pos);
			}

			if (m_sparse[pos] == nullptr)
			{
				//create a new page where every AUniqueRef is nullptr
				m_sparse.Insert(AUniqueRef<T[]>::Create(ElementPerPage), pos);
			}

			return m_sparse[pos];
		}

		ADynArr<PageType> m_sparse;
		ADynArr<T> m_packed;
		ToIntFunc m_toInt;
	};

	template<typename K, typename T>
	class ASparseSet<K, T>
	{
		static constexpr auto ElementPerPage = AE_PAGE_SIZE / sizeof(T);
		using PageType = typename AUniqueRef<K[]>;


	public:
		using AIterator = typename ADynArr<AKeyElementPair<K, T>>::AIterator;
		using AConstIterator = typename ADynArr<AKeyElementPair<K, T>>::AConstIterator;

		ASparseSet() { }

		virtual ~ASparseSet() { }

		bool Contains(const K& key) const
		{
			AE_PROFILE_FUNCTION();
			size_t page = Page(key);
			//use GetIndex to get actual index of T in packed
			return page < m_sparse.GetCount() && m_sparse[page] != nullptr
				&& (GetIndex(key) < m_packed.GetCount() && m_packed[GetIndex(key)] == key);
		}

		void Clear()
		{
			m_sparse.Clear();
			m_packed.Clear();
		}

		void ShrinkToFit()
		{
			m_packed.ShrinkToFit();
			m_sparse.ShrinkToFit();
		}

		size_t GetCount() const { return m_packed.GetCount(); }

		bool IsEmpty() const { return m_packed.IsEmpty(); }

		AIterator begin() { return m_packed.begin(); }

		AIterator end() { return m_packed.end(); }

		AConstIterator begin() const { return m_packed.begin(); }

		AConstIterator end() const { return m_packed.end(); }

		void Add(const AKeyElementPair<K, T>& pair)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(!Contains(pair.GetKey()), "Cannot add the same T twice");
			Assure(Page(pair.GetKey()))[Offset(pair.GetKey())] = (K)m_packed.GetCount();
			m_packed.Add(pair);
		}

		void Add(const K key, const T element)
		{
			AKeyElementPair<K, T> pair = AKeyElementPair<K, T>(key, element);
			Add(pair);
		}

		void Remove(const K key)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(key), "Cannot remove an element that is not contained in the SparseSet");

			size_t page = Page(key);
			size_t offset = Offset(key);

			/*move last element to the position of the element to remove
			  so that removing the element is more efficient
			  (will also need to move the element in the sparse set)
			*/
			m_packed[m_sparse[page][offset]] = T(m_packed[m_packed.GetCount() - 1]);
			m_sparse[Page(m_packed[m_packed.GetCount() - 1)].GetKey()][Offset(m_packed[m_packed.GetCount() - 1].GetKey())] = m_sparse[page][offset];
			m_sparse[page][offset] = Null;
			m_packed.Remove(m_packed.GetCount() - 1);
		}

		size_t GetIndex(const K key) const
		{
			return (size_t)m_sparse[Page(e)][Offset(e)];
		}

		void Swap(const K lhs, const K rhs)
		{
			auto& left = m_sparse[Page(lhs)][Offset(lhs)];
			auto& right = m_sparse[Page(rhs)][Offset(rhs)];
			std::swap(m_packed[size_t(left)], m_packed[size_t(right)]);
			std::swap(left, right);
		}

		const AKeyElementPair<K, T>* GetData() const
		{
			return m_packed.GetData();
		}

	private:

		size_t Page(const K key) const
		{
			return size_t{ key / ElementPerPage };
		}

		size_t Offset(const K key) const
		{
			return size_t{ key & (ElementPerPage - 1) };
		}

		PageType& Assure(const size_t pos)
		{
			AE_PROFILE_FUNCTION();
			if (!(pos < m_sparse.GetCount()))
			{
				m_sparse.Reserve(pos);
			}

			if (m_sparse[pos] == nullptr)
			{
				//create a new page where every AUniqueRef is nullptr
				m_sparse.Insert(AUniqueRef<T[]>::Create(ElementPerPage), pos);
			}

			return m_sparse[pos];
		}

		ADynArr<PageType> m_sparse;
		ADynArr<AKeyElementPair<K, T>> m_packed;
	};

}