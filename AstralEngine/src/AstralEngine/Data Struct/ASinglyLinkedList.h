#pragma once
#include "AList.h"
#include "AstralEngine/Debug/Instrumentor.h"

namespace AstralEngine
{
	template<typename T>
	class ASinglyLinkedListIterator;

	template<typename T>
	class ASinglyLinkedListConstIterator;

	template<typename T>
	class ASinglyLinkedList : public AList<T>
	{
		friend class ASinglyLinkedListIterator<T>;
		friend class ASinglyLinkedListConstIterator<T>;

	public:
		using AIterator = ASinglyLinkedListIterator<T>;
		using AConstIterator = ASinglyLinkedListConstIterator<T>;

		ASinglyLinkedList() : m_count(0), 
			m_dummy(new Node(nullptr)), m_head(m_dummy) { }

		virtual ~ASinglyLinkedList() 
		{
			AE_PROFILE_FUNCTION();
			Clear();
			delete m_dummy;
		}
		
		virtual size_t GetCount() const override { return m_count; }

		virtual void Add(T element) override
		{
			AE_PROFILE_FUNCTION();
			Node* newNode = new Node(element);
			newNode->next = m_head;
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
			Node* ptr = m_head;

			while (ptr->next != m_dummy)
			{
				ptr = ptr->next;
			}

			Node* newNode = new Node(element);
			newNode->next = m_dummy;
			ptr->next = newNode;
			m_count++;
		}

		virtual size_t Find(const T& element) const override
		{
			AE_PROFILE_FUNCTION();
			Node* ptr = m_head;

			for (size_t i = 0; i < m_count; i++)
			{
				if (ptr->element == element)
				{
					return i;
				}
				ptr = ptr->next;
			}

			return -1;
		}

		virtual void Insert(T element, size_t index) override
		{
			AE_PROFILE_FUNCTION();
			if (index == 0)
			{
				Add(element);
				return;
			}

			Node* prev = GetNode(index - 1);
			Node* newNode = new Node(element);

			newNode->next = prev->next;
			prev->next = prev;
			m_count++;
		}

		virtual void Remove(T element) override
		{
			AE_PROFILE_FUNCTION();
			Node* ptr1 = m_head;
			Node* ptr2;

			while (ptr1 != m_dummy)
			{
				if (ptr1->element == element)
				{
					break;
				}

				ptr2 = ptr1;
				ptr1 = ptr1->next;
			}
		
			if(ptr1 == m_dummy)
			{
				return;
			}

			ptr2->next = ptr1->next;
			delete ptr1;
			m_count--;
		}

		virtual void RemoveAt(size_t index) override
		{
			AE_PROFILE_FUNCTION();
			if(index == 0)
			{
				Node* ptr = m_head;
				m_head = m_head->next;
				delete ptr;
				return;
			}

			Node* prev = GetNode(index - 1);
			RemoveNode(prev);
		}

		void Remove(ASinglyLinkedList<T>::AIterator iterator)
		{
			Remove(iterator.m_currNode->element);
		}

		virtual void Reserve(size_t count) override { }
		
		virtual void Clear() override 
		{
			AE_PROFILE_FUNCTION();
			Node* ptr = m_head;
			Node* ptr2 = m_head;

			while (ptr != m_dummy)
			{
				ptr2 = ptr;
				ptr = ptr->next;
				delete ptr2;
			}
			m_head = m_dummy;
			m_count = 0;
		}

		virtual bool Contains(const T& element) const override { return Find(element) != -1; }

		virtual bool IsEmpty() const override { return m_head == m_dummy; }

		ASinglyLinkedList::AIterator begin() { return ASinglyLinkedListIterator<T>(m_head); }

		ASinglyLinkedList::AIterator end() { return ASinglyLinkedListIterator<T>(m_dummy); }
		
		ASinglyLinkedList::AConstIterator begin() const { return ASinglyLinkedListConstIterator<T>(m_head); }
		
		ASinglyLinkedList::AConstIterator end() const { return ASinglyLinkedListConstIterator<T>(m_dummy); }

		virtual T& operator[](size_t index) override { return GetNode(index)->element; }
		
		virtual const T& operator[](size_t index) const override { return GetNode(index)->element; }

		ASinglyLinkedList<T>& operator=(const ASinglyLinkedList<T>& other)
		{
			Clear();

			for (T& element : other)
			{
				Add(element);
			}
			return *this;
		}

		ASinglyLinkedList<T>& operator=(ASinglyLinkedList<T>&& other)
		{
			Clear();
			delete m_dummy;
			m_dummy = other.m_dummy;
			m_head = other.m_head;
			m_count = other.m_count;
			other.m_count = 0;
			other.m_dummy = nullptr;
			other.m_head = nullptr;
			return *this;
		}

		bool operator==(const ASinglyLinkedList<T>& other) const
		{
			ASinglyLinkedList<T>::AConstIterator it = other.begin();
			for (const T& element : *this)
			{
				if (it == other.end() || element != *it) 
				{
					return false;
				}
				it++;
			}

			return it == other.end();
		}

		bool operator!=(const ASinglyLinkedList<T>& other) const
		{
			return !(*this == other);
		}

	private:
		struct Node
		{
			T element;
			Node* next;

			Node() { }
			Node(std::nullptr_t) : next(nullptr) { }
			Node(T& e) : element(e) { }
			Node(T& e, Node* ptr) : element(e), next(ptr) { }
		};

		//takes the previous node of the node to we want to remove as argument
		void RemoveNode(Node* prevOfTarget)
		{
			Node* toDelete = prevOfTarget->next;
			prevOfTarget->next = toDelete->next;
			delete toDelete;
			m_count--;
		}

		Node* GetNode(size_t index) const
		{
			AE_PROFILE_FUNCTION();
			Node* ptr = m_head;

			for (size_t i = 0; i < index && ptr != m_dummy; i++)
			{
				ptr = ptr->next;
			}

			return ptr;
		}

		Node* m_dummy;
		Node* m_head;
		size_t m_count;
	};

	template<typename T>
	class ASinglyLinkedListIterator
	{
		friend class ASinglyLinkedList<T>;
		friend class ASinglyLinkedListConstIterator<T>;

	protected:
		using Node = typename ASinglyLinkedList<T>::Node;

	public:
		ASinglyLinkedListIterator(const ASinglyLinkedListIterator<T>& other) 
			: m_currNode(other.m_currNode) { }

		virtual ~ASinglyLinkedListIterator() { }

		ASinglyLinkedListIterator<T>& operator++()
		{
			AE_PROFILE_FUNCTION();
			m_currNode = m_currNode->next;
			return *this;
		}

		ASinglyLinkedListIterator<T>& operator+=(size_t i)
		{
			AE_PROFILE_FUNCTION();

			for (size_t j = 0; j < i; j++)
			{
				m_currNode = m_currNode->next;
			}

			return *this;
		}

		ASinglyLinkedListIterator<T> operator++(int)
		{
			AE_PROFILE_FUNCTION();
			ASinglyLinkedListIterator<T> it = *this;
			this->operator++();
			return it;
		}

		bool operator==(const ASinglyLinkedListIterator<T>& other) const
		{
			return m_currNode == other.m_currNode;
		}

		bool operator!=(const ASinglyLinkedListIterator<T>& other) const
		{
			return !(*this == other);
		}

		T& operator*()
		{
			return m_currNode->element;
		}

	private:
		ASinglyLinkedListIterator(Node* node) : m_currNode(node) { }


		Node* m_currNode;
	};

	template<typename T>
	class ASinglyLinkedListConstIterator : public ASinglyLinkedListIterator<T>
	{
		friend class ASinglyLinkedList<T>;
	public:
		ASinglyLinkedListConstIterator(const ASinglyLinkedListConstIterator<T>& other)
			: ASinglyLinkedListIterator<T>(other) { }
		
		ASinglyLinkedListIterator<T>& operator++()
		{
			ASinglyLinkedListIterator<T>::operator++();
			return *this;
		}

		ASinglyLinkedListIterator<T>& operator+=(size_t i)
		{
			ASinglyLinkedListIterator<T>::operator+=(i);
			return *this;
		}

		ASinglyLinkedListConstIterator<T> operator++(int)
		{
			AE_PROFILE_FUNCTION();
			ASinglyLinkedListConstIterator<T> it = *this;
			this->operator++();
			return it;
		}

		bool operator==(const ASinglyLinkedListConstIterator<T>& other) const
		{
			return ASinglyLinkedListIterator<T>::operator==(other);
		}

		bool operator!=(const ASinglyLinkedListConstIterator<T>& other) const
		{
			return !(*this == other);
		}

		const T& operator*() const
		{
			return const_cast<ASinglyLinkedListConstIterator<T>*>(this)->ASinglyLinkedListIterator<T>::operator*();
		}

	private:
		ASinglyLinkedListConstIterator(ASinglyLinkedListIterator<T>::Node* node)
			: ASinglyLinkedListIterator<T>(node) { }

	};

}
