#pragma once
#include "AList.h"
#include "AstralEngine/Debug/Instrumentor.h"

namespace AstralEngine
{
	template<typename T>
	class ADoublyLinkedListIterator;

	template<typename T>
	class ADoublyLinkedListConstIterator;

	template<typename T>
	class ADoublyLinkedList : public AList<T>
	{
		struct Node;
		friend class ADoublyLinkedListIterator<T>;
		friend class ADoublyLinkedListConstIterator<T>;

	public:
		using AIterator = ADoublyLinkedListIterator<T>;
		using AConstIterator = ADoublyLinkedListConstIterator<T>;

		ADoublyLinkedList() : m_count(0)
		{
			AE_PROFILE_FUNCTION();

			m_dummy = new Node();
			m_head = m_dummy;
		}


		ADoublyLinkedList(const std::initializer_list<T>& list) : m_count(0)
		{
			AE_PROFILE_FUNCTION();

			m_dummy = new Node();
			m_head = m_dummy;

			for (typename std::initializer_list<T>::iterator it = list.begin(); it != list.end(); it++)
			{
				AddLast(*it);
			}
		}

		ADoublyLinkedList(const ADoublyLinkedList<T>& other) : m_count(other.m_count)
		{
			AE_PROFILE_FUNCTION();

			m_dummy = new Node();
			m_head = m_dummy;

			for (AIterator it = other.rbegin(); it != other.rend(); it--)
			{
				Add(*it);
			}
		}

		ADoublyLinkedList(ADoublyLinkedList<T>&& other) noexcept 
			: m_count(other.m_count), m_dummy(other.m_dummy), m_head(other.m_head)
		{
			AE_PROFILE_FUNCTION();

			other.m_dummy = nullptr;
			other.m_head = nullptr;
			other.m_count = 0;
		}

		virtual ~ADoublyLinkedList() 
		{
			AE_PROFILE_FUNCTION();

			DeleteList();
			delete m_dummy;
		}

		virtual size_t GetCount() const override
		{
			return m_count;
		}

		virtual void Add(T element) override
		{
			AE_PROFILE_FUNCTION();

			Node* newNode = new Node();
			newNode->element = element;

			newNode->next = m_head;
			m_head->prev = newNode;
			m_head = newNode;

			m_count++;
		}
		
		//emplace element at the end of the list
		template<typename... Args>
		T& Emplace(Args... args)
		{
			AE_PROFILE_FUNCTION();
			Node* newNode = new Node();

			newNode->element = T(std::forward<Args>(args)...);

			newNode->next = m_dummy;

			if (m_dummy != m_head)
			{
				newNode->prev = m_dummy->prev;
				m_dummy->prev->next = newNode;
			}
			else
			{
				m_head = newNode;
			}
			m_dummy->prev = newNode;


			m_count++;

			return newNode->element;
		}

		//moves the element into the list
		void MoveAdd(T&& element)
		{
			AE_PROFILE_FUNCTION();

			Node* newNode = new Node();
			newNode->element = std::move(element);

			newNode->next = m_head;
			m_head->prev = newNode;
			m_head = newNode;

			m_count++;
		}

		virtual void AddFirst(T element) override
		{
			Add(element);
		}

		virtual void AddLast(T element) override
		{
			AE_PROFILE_FUNCTION();

			Node* newNode = new Node();
			newNode->element = element;

			newNode->next = m_dummy;

			if (m_dummy != m_head)
			{
				newNode->prev = m_dummy->prev;
				m_dummy->prev->next = newNode;
			}
			else
			{
				m_head = newNode;
			}
			m_dummy->prev = newNode;
			

			m_count++;
		}

		size_t Find(const T& element) const override
		{
			AE_PROFILE_FUNCTION();

			size_t index = 0;
			for (ADoublyLinkedList<T>::AIterator it = begin(); it != end(); it++)
			{
				if (*it == element)
				{
					return index;
				}
				index++;
			}
			return -1;
		}

		virtual void Insert(T element, size_t index) override
		{
			AE_PROFILE_FUNCTION();

			Node* indexNode = GetNode(index);
			Node* newNode = new Node();
			newNode->element = element;

			if (indexNode == m_head)
			{
				newNode->next = m_head;
				m_head->prev = newNode;
				m_head = newNode;
			}
			else
			{
				newNode->next = indexNode;
				newNode->prev = indexNode->prev;
				indexNode->prev->next = newNode;
				indexNode->prev = newNode;
			}
			m_count++;
		}

		void Insert(T element, AIterator it)
		{
			AE_PROFILE_FUNCTION();

			Node* indexNode = it.m_currNode;
			Node* newNode = new Node();
			newNode->element = element;

			if (indexNode == m_head)
			{
				newNode->next = m_head;
				m_head->prev = newNode;
				m_head = newNode;
			}
			else
			{
				newNode->next = indexNode;
				newNode->prev = indexNode->prev;
				indexNode->prev->next = newNode;
				indexNode->prev = newNode;
			}
			m_count++;
		}

		void Insert(T element, AConstIterator it)
		{
			AE_PROFILE_FUNCTION();

			Node* indexNode = it.m_currNode;
			Node* newNode = new Node();
			newNode->element = element;

			if (indexNode == m_head)
			{
				newNode->next = m_head;
				m_head->prev = newNode;
				m_head = newNode;
			}
			else
			{
				newNode->next = indexNode;
				newNode->prev = indexNode->prev;
				indexNode->prev->next = newNode;
				indexNode->prev = newNode;
			}
			m_count++;
		}

		virtual void Remove(T element) override
		{
			AE_PROFILE_FUNCTION();

			Node* ptr = m_head;

			while(ptr != m_dummy)
			{
				if (ptr->element == element)
				{
					if (ptr != m_head)
					{
						ptr->prev->next = ptr->next;
						ptr->next->prev = ptr->prev;
					}
					else
					{
						m_head = ptr->next;
					}
					delete ptr;

					m_count--;
					break;
				}
				ptr = ptr->next;
			}
		}

		virtual void RemoveAt(size_t index) override
		{
			AE_PROFILE_FUNCTION();

			Node* toRemove = GetNode(index);
			RemoveNode(toRemove);
		}

		virtual void Remove(AIterator iterator)
		{
			RemoveNode(iterator.m_currNode);
		}

		virtual void Reserve(size_t count) override { };

		virtual void Clear() override
		{
			AE_PROFILE_FUNCTION();

			DeleteList();
			m_head = m_dummy;
			m_count = 0;
		}

		virtual T& operator[](size_t index) override
		{
			return GetNode(index)->element;
		}

		virtual bool Contains(const T& element) const override
		{
			return Find(element) != -1;
		}

		virtual const T& operator[](size_t index) const override
		{
			Node* n = GetNode(index);
			return n->element;
		}

		ADoublyLinkedList<T>::AIterator begin()
		{
			return ADoublyLinkedListIterator<T>(m_head);
		}

		ADoublyLinkedList<T>::AIterator end()
		{
			return ADoublyLinkedListIterator<T>(m_dummy);
		}

		ADoublyLinkedList<T>::AIterator rbegin()
		{
			return ADoublyLinkedListIterator<T>(m_dummy->prev);
		}

		ADoublyLinkedList<T>::AIterator rend()
		{
			return ADoublyLinkedListIterator<T>(m_head->prev);
		}

		ADoublyLinkedList<T>::AConstIterator begin() const
		{
			return ADoublyLinkedListConstIterator<T>(m_head);
		}

		ADoublyLinkedList<T>::AConstIterator end() const
		{
			return ADoublyLinkedListConstIterator<T>(m_dummy);
		}

		ADoublyLinkedList<T>::AConstIterator rbegin() const
		{
			return ADoublyLinkedListConstIterator<T>(m_dummy->prev);
		}

		ADoublyLinkedList<T>::AIterator rend() const
		{
			return ADoublyLinkedListConstIterator<T>(m_head->prev);
		}
		

		ADoublyLinkedList<T>& operator=(const ADoublyLinkedList<T>& other)
		{
			AE_PROFILE_FUNCTION();

			DeleteList();

			m_head = m_dummy;
			m_count = 0;

			for (AIterator it = other.rbegin(); it != other.rend(); it--)
			{
				Add(*it);
			}

			return *this;
		}

		ADoublyLinkedList<T>& operator=(ADoublyLinkedList<T>&& other) noexcept
		{
			AE_PROFILE_FUNCTION();

			DeleteList();

			delete m_dummy;

			m_head = other.m_head;
			m_count = other.m_count;
			m_dummy = other.m_dummy;

			other.m_count = 0;
			other.m_head = nullptr;
			other.m_dummy = nullptr;

			return *this;
		}

		bool operator==(const ADoublyLinkedList<T>& other) const
		{
			AE_PROFILE_FUNCTION();
			if (other.m_count != m_count)
			{
				return false;
			}

			Node* p1 = m_head;
			Node* p2 = other.m_head;

			while (p1 != m_dummy)
			{
				if (p1->element != p2->element)
				{
					return false;
				}

				AE_CORE_ASSERT(p2 != other.m_dummy, "Iterator Desynch");
				p1 = p1->next;
				p2 = p2->next;

			}

			return true;
		}

	private:

		Node* GetNode(size_t index) const
		{
			AE_PROFILE_FUNCTION();

			if (m_count == 0)
			{
				AE_CORE_ASSERT(index == 0, "Invalid Index provided");
				return m_head;
			}

			Node* ptr;
			if (index < m_count / 2)
			{
				ptr = m_head;
				for (size_t i = 0; i < index; i++)
				{
					ptr = ptr->next;
				}
			}
			else
			{
				ptr = m_dummy->prev;
				index = m_count - index - 1;
				for (size_t i = 0; i < index; i++)
				{
					ptr = ptr->prev;
				}
			}

			return ptr;
		}

		void DeleteList()
		{
			AE_PROFILE_FUNCTION();
		
			Node* ptr1 = m_head;
			Node* ptr2;
			while (ptr1 != m_dummy)
			{
				ptr2 = ptr1;
				ptr1 = ptr1->next;
				delete ptr2;
			}
		}

		void RemoveNode(Node* toRemove)
		{
			if (toRemove != m_head)
			{
				toRemove->prev->next = toRemove->next;
				toRemove->next->prev = toRemove->prev;
			}
			else
			{
				m_head = toRemove->next;
			}
			delete toRemove;

			m_count--;
		}

		struct Node
		{
			T element;
			Node* next;
			Node* prev;
		};

		Node* m_head;
		Node* m_dummy;
		size_t m_count;
	};

	template<typename T>
	class ADoublyLinkedListIterator 
	{
		friend class ADoublyLinkedList<T>;
	public:

		ADoublyLinkedListIterator(typename ADoublyLinkedList<T>::Node* node) : m_currNode(node) { }

		ADoublyLinkedListIterator(const ADoublyLinkedListIterator<T>& other) : m_currNode(other.m_currNode) { }

		virtual ~ADoublyLinkedListIterator() { }

		ADoublyLinkedListIterator<T>& operator++()
		{
			AE_PROFILE_FUNCTION();
			m_currNode = m_currNode->next;
			return *this;
		}

		ADoublyLinkedListIterator<T>& operator+=(size_t i)
		{
			AE_PROFILE_FUNCTION();

			for (size_t j = 0; j < i; j++)
			{
				m_currNode = m_currNode->next;
			}

			return *this;
		}

		ADoublyLinkedListIterator<T> operator++(int)
		{
			AE_PROFILE_FUNCTION();
			ADoublyLinkedListIterator<T> it = *this;
			this->operator++();
			return it;
		}

		ADoublyLinkedListIterator<T>& operator--()
		{
			m_currNode = m_currNode->prev;
			return *this;
		}

		ADoublyLinkedListIterator<T>& operator-=(size_t i)
		{
			AE_PROFILE_FUNCTION();
			
			for (size_t j = 0; j < i; j++)
			{
				m_currNode = m_currNode->prev;
			}
			return *this;
		}

		ADoublyLinkedListIterator<T> operator--(int)
		{
			AE_PROFILE_FUNCTION();
			ADoublyLinkedListIterator<T> it = *this;
			this->operator--();
			return it;
		}

		bool operator==(const ADoublyLinkedListIterator<T>& other) const
		{
			return m_currNode == other.m_currNode;
		}

		bool operator!=(const ADoublyLinkedListIterator<T>& other) const
		{
			return !(*this == other);
		}

		T& operator*()
		{
			return m_currNode->element;
		}

	private:		
		typename ADoublyLinkedList<T>::Node* m_currNode;
	};

	template<typename T>
	class ADoublyLinkedListConstIterator : public ADoublyLinkedListIterator<T>
	{
	public:
		
		ADoublyLinkedListConstIterator(typename ADoublyLinkedList<T>::Node* node) 
			: ADoublyLinkedListIterator<T>(node) { }
		
		ADoublyLinkedListConstIterator(const ADoublyLinkedListConstIterator<T>& other) 
			: ADoublyLinkedListIterator<T>(other) { }
		
		ADoublyLinkedListConstIterator<T>& operator++()
		{
			ADoublyLinkedListIterator<T>::operator++();
			return *this;
		}

		ADoublyLinkedListConstIterator<T>& operator+=(size_t i)
		{
			ADoublyLinkedListIterator<T>::operator+=(i);
			return *this;
		}

		ADoublyLinkedListConstIterator<T> operator++(int)
		{
			AE_PROFILE_FUNCTION();
			ADoublyLinkedListConstIterator<T> it = *this;
			this->operator++();
			return it;
		}

		ADoublyLinkedListConstIterator<T>& operator--()
		{
			ADoublyLinkedListIterator<T>::operator--();
			return *this;
		}

		ADoublyLinkedListConstIterator<T>& operator-=(size_t i)
		{
			ADoublyLinkedListIterator<T>::operator-=(i);
			return *this;
		}

		ADoublyLinkedListConstIterator<T> operator--(int)
		{
			AE_PROFILE_FUNCTION();
			ADoublyLinkedListConstIterator<T> it = *this;
			this->operator--();
			return it;
		}

		bool operator==(const ADoublyLinkedListConstIterator<T>& other) const
		{
			return ADoublyLinkedListIterator<T>::operator==(other);
		}

		bool operator!=(const ADoublyLinkedListConstIterator<T>& other) const
		{
			return !(*this == other);
		}

		const T& operator*() const
		{
			return ADoublyLinkedListIterator<T>::operator*();
		}

	};


}