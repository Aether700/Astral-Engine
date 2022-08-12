#pragma once
#include "AstralEngine/Core/Core.h"
#include "AstralEngine/Debug/Instrumentor.h"


namespace AstralEngine
{
	template<typename T>
	class ADynArr;

	template<typename T>
	class ADynArrIterator
	{
		friend class ADynArr<T>;
	public:
		ADynArrIterator(const ADynArrIterator<T>& other) : m_pos(other.m_pos), m_arr(other.m_arr) { }

		virtual ~ADynArrIterator() { }

		ADynArrIterator<T>& operator++()
		{
			m_pos++;
			return *this;
		}

		ADynArrIterator<T>& operator+=(size_t i)
		{
			m_pos += i;
			return *this;
		}

		ADynArrIterator<T> operator++(int)
		{
			ADynArrIterator<T> copy = *this;
			this->operator++();
			return copy;
		}

		ADynArrIterator<T>& operator--()
		{
			m_pos--;
			return *this;
		}

		ADynArrIterator<T>& operator-=(size_t i)
		{
			m_pos -= i;
			return *this;
		}

		ADynArrIterator<T> operator--(int)
		{
			ADynArrIterator<T> copy = *this;
			this->operator--();
			return copy;
		}

		bool operator==(const ADynArrIterator<T>& other) const
		{
			return m_pos == other.m_pos && m_arr == other.m_arr;
		}

		bool operator!=(const ADynArrIterator<T>& other) const
		{
			return !(*this == other);
		}

		T& operator*() const
		{
			return m_arr->Get(m_pos);
		}

	protected:
		ADynArrIterator(size_t pos, ADynArr<T>* arr) : m_pos(pos), m_arr(arr) { }

	private:
		size_t m_pos;
		ADynArr<T>* m_arr;
	};

	template<typename T>
	class ADynArrConstIterator : public ADynArrIterator<T>
	{
		friend class ADynArr<T>;
	public:
		ADynArrConstIterator(const ADynArrConstIterator<T>& other) : ADynArrIterator<T>(other) { }

		virtual ADynArrConstIterator<T>& operator++()
		{
			ADynArrIterator<T>::operator++();
			return *this;
		}

		virtual ADynArrConstIterator<T>& operator+=(size_t i)
		{
			ADynArrIterator<T>::operator+=(i);
			return *this;
		}

		virtual ADynArrConstIterator<T> operator++(int)
		{
			ADynArrConstIterator<T> it = *this;
			this->operator++();
			return it;
		}

		virtual ADynArrConstIterator<T>& operator--()
		{
			ADynArrIterator<T>::operator--();
			return *this;
		}

		virtual ADynArrConstIterator<T>& operator-=(size_t i)
		{
			ADynArrIterator<T>::operator-=(i);
			return *this;
		}

		virtual ADynArrConstIterator<T> operator--(int)
		{
			AE_PROFILE_FUNCTION();
			ADynArrConstIterator<T> it = *this;
			this->operator--();
			return it;
		}

		virtual bool operator==(const ADynArrConstIterator<T>& other) const
		{
			return ADynArrIterator<T>::operator==(other);
		}

		virtual bool operator!=(const ADynArrConstIterator<T>& other) const
		{
			return !(*this == other);
		}

		const T& operator*() const
		{
			return ADynArrIterator<T>::operator*();
		}

	private:
		ADynArrConstIterator(size_t pos, const ADynArr<T>* arr) : ADynArrIterator<T>(pos, const_cast<ADynArr<T>*>(arr)) { }
	};

	template<typename T>
	class ADynArr sealed
	{
		friend class ADynArrIterator<T>;
	public:
		using AIterator = ADynArrIterator<T>;
		using AConstIterator = ADynArrConstIterator<T>;

		ADynArr(const std::initializer_list<T>& list, size_t reserve = 0) : m_maxCount(list.size() + reserve), m_count(list.size())
		{
			m_arr = new T[list.size() + reserve];

			size_t i = 0;
			for (typename std::initializer_list<T>::iterator it = list.begin(); it != list.end(); it++)
			{
				m_arr[i] = *it;
				i++;
			}
		}
		
		ADynArr(size_t startMax = 5) : m_maxCount(startMax), m_count(0)
		{
			m_arr = new T[m_maxCount];
		}

		ADynArr(const ADynArr<T>& other) : m_count(other.m_count), m_maxCount(other.m_maxCount)
		{
			m_arr = new T[m_maxCount];

			for (size_t i = 0; i < other.GetCount(); i++)
			{
				m_arr[i] = other[i];
			}
		}

		ADynArr(ADynArr<T>&& other) : m_count(other.m_count), m_maxCount(other.m_maxCount)
		{
			m_arr = other.m_arr;

			other.m_arr = nullptr;
			other.m_count = 0;
		}

		~ADynArr()
		{
			delete[] m_arr;
		}

		size_t GetCount() const 
		{
			return m_count; 
		}

		size_t GetCapacity() const
		{
			return m_maxCount;
		}

		void Add(T&& element)
		{
			AE_PROFILE_FUNCTION();
			CheckSize();
			m_arr[m_count] = std::move(element);
			m_count++;
		}

		void Add(const T& element)
		{
			AE_PROFILE_FUNCTION();
			CheckSize();
			m_arr[m_count] = element;
			m_count++;
		}

		template<typename... Args>
		T& Emplace(Args&&... args)
		{
			AE_PROFILE_FUNCTION();
			CheckSize();

			for (size_t i = m_count; i > 0; i--)
			{
				m_arr[i] = m_arr[i - 1];
			}
			m_arr[0] = T(std::forward<Args>(args)...);
			m_count++;

			return m_arr[0];
		}

		void AddFirst(T&& element)
		{
			AE_PROFILE_FUNCTION();
			Insert(std::move(element), 0);
		}

		void AddFirst(const T& element) 
		{
			AE_PROFILE_FUNCTION();
			Insert(element, 0);	
		}

		void AddLast(T&& element)
		{
			Add(std::forward<T>(element));
		}

		void AddLast(const T& element) 
		{
			Add(element); 
		}

		template<typename... Args>
		T& EmplaceBack(Args&&... args)
		{
			AE_PROFILE_FUNCTION();
			CheckSize();
			m_arr[m_count] = T(std::forward<Args>(args)...);
			m_count++;

			return m_arr[m_count - 1];
		}

		template<typename... Args>
		T& EmplaceAt(size_t index, Args&&... args)
		{
			AE_PROFILE_FUNCTION();
			CheckSize();

			if (m_count == 0)
			{
				AE_CORE_ASSERT(index == 0, "Invalid index provided");
				return EmplaceBack(std::forward<Args>(args)...);
			}

			for (size_t i = m_count - 1; i >= index; i--)
			{
				m_arr[i + 1] = std::move(m_arr[i]);
			}
			m_arr[index] = T(std::forward<Args>(args)...);
			m_count++;
			return m_arr[index];
		}
		
		size_t Find(const T& element) const
		{
			AE_PROFILE_FUNCTION();
			for (size_t i = 0; i < m_count; i++)
			{
				if (m_arr[i] == element)
				{
					return i;
				}
			}
			return -1;
		}

		void Insert(const T& element, AIterator it)
		{
			Insert(element, it.m_pos);
		}

		void Insert(const T& element, AConstIterator it)
		{
			Insert(element, it.m_pos);
		}

		void Insert(T&& element, size_t index)
		{
			AE_PROFILE_FUNCTION();
			CheckSize();

			if (m_count == 0)
			{
				AE_CORE_ASSERT(index == 0, "Invalid index provided");
				Add(element);
				return;
			}

			for (size_t i = m_count - 1; i >= index; i--)
			{
				m_arr[i + 1] = std::move(m_arr[i]);
			}
			m_arr[index] = std::move(element);
			m_count++;
		}

		void Insert(const T& element, size_t index)
		{
			AE_PROFILE_FUNCTION();
			CheckSize();
			
			if (m_count == 0)
			{
				AE_CORE_ASSERT(index == 0, "Invalid index provided");
				Add(element);
				return;
			}

			for (size_t i = m_count - 1; i >= index; i--)
			{
				m_arr[i + 1] = std::move(m_arr[i]);
			}
			m_arr[index] = element;
			m_count++;
		}

		void Insert(T&& element, const AIterator& it)
		{
			Insert(std::forward(element), it.m_pos);
		}

		void Insert(T&& element, const AConstIterator& it)
		{
			Insert(std::forward(element), it.m_pos);
		}

		void Remove(const T& element)
		{
			AE_PROFILE_FUNCTION();
			size_t index = Find(element);
			if (index != -1)
			{
				RemoveAt(index);
			}
		}

		void RemoveAt(size_t index)
		{
			AE_PROFILE_FUNCTION();
			
			if (index == m_count - 1)
			{
				m_count--;
				return;
			}

			for (size_t i = index; i < m_count; i++)
			{
				m_arr[i] = std::move(m_arr[i + 1]);
			}
			m_count--;
		}

		void Remove(ADynArr<T>::AIterator iterator)
		{
			int index = (iterator.m_ptr - m_arr) / sizeof(T);
			Remove(index);
		}

		void ShrinkToFit()
		{
			T* newArr = new T[m_count];

			for (size_t i = 0; i < m_count; i++)
			{
				newArr[i] == m_arr[i];
			}

			delete[] m_arr;
			m_arr = newArr;
			m_maxCount = m_count;
		}

		void Reserve(size_t count)
		{
			AE_PROFILE_FUNCTION();
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

		void Clear()
		{
			m_count = 0;
		}

		bool Contains(const T& element) const
		{
			return Find(element) != -1;
		}

		bool IsEmpty() const { return m_count == 0; }

		T& Get(size_t index)
		{
			return this->operator[](index);
		}

		const T& Get(size_t index) const
		{
			return this->operator[](index);
		}

		T* GetData() { return m_arr; }

		const T* GetData() const { return m_arr; }
		
		AIterator begin()
		{
			return ADynArrIterator<T>(0, this); 
		}
		
		AIterator end()
		{
			return ADynArrIterator<T>(m_count, this); 
		}

		AIterator rbegin() 
		{
			return ADynArrIterator<T>(m_count - 1, this);
		}
		
		AIterator rend() 
		{
			return ADynArrIterator<T>(-1, this);
		}

		AConstIterator begin() const
		{
			return ADynArrConstIterator<T>(0, this);
		}

		AConstIterator end() const
		{
			return ADynArrConstIterator<T>(m_count, this);
		}

		AConstIterator rbegin() const
		{
			return ADynArrConstIterator<T>(m_count - 1, this);
		}

		AConstIterator rend() const
		{
			return ADynArrConstIterator<T>(-1, this);
		}


		T& operator[](size_t index) 
		{
			AE_CORE_ASSERT(index >= 0 && index < m_count, "Index out of bounds");
			return m_arr[index]; 
		}
		
		const T& operator[](size_t index) const
		{
			AE_CORE_ASSERT(index >= 0 && index < m_count, "Index out of bounds");
			return m_arr[index];
		}

		bool operator==(const ADynArr<T>& other) const
		{
			AE_PROFILE_FUNCTION();
			if (GetCount() != other.GetCount())
			{
				return false;
			}

			for (size_t i = 0; i < GetCount(); i++)
			{
				if (!(m_arr[i] == other[i]))
				{
					return false;
				}
			}
			return true;
		}

		bool operator!=(const ADynArr<T>& other) const 
		{
			AE_PROFILE_FUNCTION(); 
			return !(*this == other); 
		}

		ADynArr<T>& operator=(const ADynArr<T>& other) 
		{
			if (m_maxCount != other.m_maxCount)
			{
				delete[] m_arr;
				m_arr = new T[other.m_maxCount];
			}


			for (size_t i = 0 ; i < other.GetCount(); i++)
			{
				m_arr[i] = std::move(other[i]);
			}

			m_count = other.m_count;
			m_maxCount = other.m_maxCount;
			return *this;
		}

		ADynArr<T>& operator=(ADynArr<T>&& other)
		{
			delete[] m_arr;

			m_arr = other.m_arr;
			m_count = other.m_count;
			m_maxCount = other.m_maxCount;

			other.m_arr = nullptr;
			other.m_count = 0;
			return *this;
		}

	private:
		
		void CheckSize()
		{
			AE_PROFILE_FUNCTION();
			if (m_count >= m_maxCount)
			{
				Resize();
			}
		}

		void Resize()
		{
			AE_PROFILE_FUNCTION();
			size_t newMax = (size_t)((float)m_maxCount * 1.5f) + (size_t)1;
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

}