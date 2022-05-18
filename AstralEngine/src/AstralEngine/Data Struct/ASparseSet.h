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

	//Similar ADynArr but the data does not have to be contiguous and can be spread 
	//throughout the array with indices that are unassigned in the middle of the array
	template<typename T>
	class ResizableArr
	{
	public:
		class AIterator;
		class AConstIterator;

		ResizableArr(size_t startSize = 5) 
			: m_arr(new T[startSize]), m_count(0), m_maxCount(startSize) { }

		~ResizableArr()
		{
			delete[] m_arr;
		}

		void Add(const T& element)
		{
			CheckSize();
			m_arr[m_count] = element;
			m_count++;
		}

		void Add(T& element)
		{
			CheckSize();
			m_arr[m_count] = element;
			m_count++;
		}

		// Returns the index of the largest valid index
		size_t GetCount() const
		{
			return m_count;
		}

		T* GetData() { return m_arr; }

		const T* GetData() const { return m_arr; }

		size_t GetCapacity() const
		{
			return m_maxCount;
		}

		void RemoveAt(size_t index)
		{
			if (index == m_count - 1)
			{
				m_count--;
				return;
			}

			for (size_t i = index; i < m_count; i++)
			{
				m_arr[i] = m_arr[i + 1];
			}
			m_count--;
		}

		T& operator[](size_t index)
		{
			if (index >= m_count)
			{
				Reserve(index + 2);
				m_count = index + 1;
			}
			return m_arr[index];
		}

		const T& operator[](size_t index) const
		{
			return m_arr[index];
		}

		void Reserve(size_t count)
		{
			size_t currentCount = m_maxCount - m_count;
			if (currentCount < count)
			{
				size_t newMax = m_maxCount + count - currentCount;
				T* temp = new T[newMax];

				for (size_t i = 0; i < m_count; i++)
				{
					temp[i] = std::move(m_arr[i]);
				}

				delete[] m_arr;
				m_arr = temp;
				m_maxCount = newMax;
			}
		}

		ResizableArr<T>::AIterator begin()
		{
			return AIterator(0, m_arr);
		}

		ResizableArr<T>::AIterator end()
		{
			return AIterator(m_count, m_arr);
		}

		ResizableArr<T>::AIterator rbegin()
		{
			return AIterator(m_count - 1, m_arr);
		}

		ResizableArr<T>::AIterator rend()
		{
			return AIterator(-1, m_arr);
		}

		ResizableArr<T>::AConstIterator begin() const
		{
			return AConstIterator(0, m_arr);
		}

		ResizableArr<T>::AConstIterator end() const
		{
			return AConstIterator(m_count, m_arr);
		}

		ResizableArr<T>::AConstIterator rbegin() const
		{
			return AConstIterator(m_count - 1, m_arr);
		}

		ResizableArr<T>::AConstIterator rend() const
		{
			return AConstIterator(-1, m_arr);
		}
		
		class AIterator
		{
			friend class ResizableArr<T>;
		public:
			AIterator(const AIterator& other) : m_pos(other.m_pos), m_arr(other.m_arr) { }

			virtual ~AIterator() { }

			AIterator& operator++()
			{
				m_pos++;
				return *this;
			}

			AIterator& operator+=(size_t i)
			{
				m_pos += i;
				return *this;
			}

			AIterator operator++(int)
			{
				AIterator copy = *this;
				this->operator++();
				return copy;
			}

			AIterator& operator--()
			{
				m_pos--;
				return *this;
			}

			AIterator& operator-=(size_t i)
			{
				m_pos -= i;
				return *this;
			}

			AIterator operator--(int)
			{
				AIterator copy = *this;
				this->operator--();
				return copy;
			}

			bool operator==(const AIterator& other) const
			{
				return m_pos == other.m_pos && m_arr == other.m_arr;
			}

			bool operator!=(const AIterator& other) const
			{
				return !(*this == other);
			}

			T& operator*() const
			{
				return m_arr[m_pos];
			}

		protected:
			AIterator(size_t pos, T* arr) : m_pos(pos), m_arr(arr) { }


		private:
			size_t m_pos;
			T* m_arr;
		};

		class AConstIterator : public AIterator
		{
			friend class ResizableArr<T>;
		public:
			AConstIterator(const AConstIterator& other) : AIterator(other) { }
			
			virtual AConstIterator& operator++()
			{
				AIterator::operator++();
				return *this;
			}

			virtual AConstIterator& operator+=(size_t i)
			{
				AIterator::operator+=(i);
				return *this;
			}

			virtual AConstIterator operator++(int)
			{
				AConstIterator it = *this;
				this->operator++();
				return it;
			}

			virtual AConstIterator& operator--()
			{
				AIterator::operator--();
				return *this;
			}

			virtual AConstIterator& operator-=(size_t i)
			{
				AIterator::operator-=(i);
				return *this;
			}

			virtual AConstIterator operator--(int)
			{
				AConstIterator it = *this;
				this->operator--();
				return it;
			}

			virtual bool operator==(const AConstIterator& other) const
			{
				return AIterator::operator==(other);
			}

			virtual bool operator!=(const AConstIterator& other) const
			{
				return !(*this == other);
			}

			const T& operator*() const
			{
				return AIterator::operator*();
			}

		private:
			AConstIterator(size_t pos, T* arr) : AIterator(pos, arr) { }

		};

		ResizableArr<T>& operator=(const ResizableArr<T>& other)
		{
			delete[] m_arr;
			m_arr = new T[other.m_maxCount];
			for (int i = 0; i < other.m_count; i++)
			{
				m_arr[i] = other.m_arr[i];
			}

			m_count = other.m_count;
			m_maxCount = other.m_maxCount;
			return *this;
		}

	private:
		void CheckSize()
		{
			if (m_count >= m_maxCount)
			{
				Resize();
			}
		}

		void Resize()
		{
			size_t newMax = (size_t)((float)m_maxCount * 1.5f) + 1;
			T* temp = new T[newMax];

			for (size_t i = 0; i < m_count; i++)
			{
				temp[i] = std::move(m_arr[i]);
			}

			delete[] m_arr;
			m_arr = temp;
			m_maxCount = newMax;
		}


		T* m_arr;
		size_t m_count;
		size_t m_maxCount;
	};

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

		ASparseSet(const ASparseSet<T>& other) 
			: m_packed(other.m_packed), m_sparse(other.m_sparse), m_toInt(other.m_toInt) { }

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
			AE_CORE_ASSERT(!Contains(e), "Cannot add the same element twice");
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
			  (also need to move the element in the sparse set)
			*/
			T lastElement = m_packed[m_packed.GetCount() - 1];

			m_packed[m_toInt(m_sparse[page][offset])] = lastElement;
			m_sparse[Page(lastElement)][Offset(lastElement)] = m_sparse[page][offset];
			m_sparse[page][offset] = Null;
			m_packed.RemoveAt(m_packed.GetCount() - 1);
		}

		size_t GetIndex(const T e) const
		{
			AE_CORE_ASSERT(Contains(e), "Trying to get index of an object not in the sparse set");
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

		ASparseSet<T>& operator=(const ASparseSet<T>& other)
		{
			m_sparse = other.m_sparse;
			m_packed = other.m_packed;
			m_toInt = other.m_toInt;
			return *this;
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
				m_sparse[pos] = AUniqueRef<T[]>::Create(ElementPerPage);
			}

			return m_sparse[pos];
		}

		ResizableArr<PageType> m_sparse;
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
			m_sparse[Page(m_packed[m_packed.GetCount() - 1].GetKey())][Offset(m_packed[m_packed.GetCount() - 1].GetKey())] = m_sparse[page][offset];
			m_sparse[page][offset] = Null;
			m_packed.Remove(m_packed.GetCount() - 1);
		}

		size_t GetIndex(const K key) const
		{
			return (size_t)m_sparse[Page(key)][Offset(key)];
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
				m_sparse[pos] = AUniqueRef<T[]>::Create(ElementPerPage);
			}

			return m_sparse[pos];
		}

		ADynArr<PageType> m_sparse;
		ADynArr<AKeyElementPair<K, T>> m_packed;
	};

}