#pragma once
#include "AstralEngine/Debug/Instrumentor.h"

namespace AstralEngine
{
	template<typename T>
	class ASinglyLinkedListIterator;

	template<typename T>
	class ASinglyLinkedListConstIterator;

	template<typename T>
	class ASinglyLinkedList sealed
	{
		friend class ASinglyLinkedListIterator<T>;
		friend class ASinglyLinkedListConstIterator<T>;

	public:
		using AIterator = ASinglyLinkedListIterator<T>;
		using AConstIterator = ASinglyLinkedListConstIterator<T>;

		ASinglyLinkedList() : m_count(0), 
			m_dummy(new Node(nullptr)), m_head(m_dummy) { }

		ASinglyLinkedList(const ASinglyLinkedList& other) : m_count(0),
			m_dummy(new Node(nullptr)), m_head(m_dummy)
		{
			for(T element : other)
			{
				Add(element);
			}
		}

		ASinglyLinkedList(ASinglyLinkedList&& other) : m_count(other.m_count),
			m_dummy(other.m_dummy), m_head(other.m_head)
		{
			other.m_head = nullptr;
			other.m_dummy = nullptr;
			other.m_count = 0;
		}

		~ASinglyLinkedList() 
		{
			
			Clear();
			delete m_dummy;
		}
		
		size_t GetCount() const { return m_count; }

		void Add(T&& element)
		{
			
			Node* newNode = new Node(std::forward<T>(element));
			AddNode(newNode);
		}

		void Add(const T& element)
		{
			
			Node* newNode = new Node(element);
			AddNode(newNode);
		}

		void AddFirst(T&& element)
		{
			Add(std::forward<T>(element));
		}

		void AddFirst(const T& element)
		{
			Add(element);
		}

		void AddLast(T&& element)
		{
			
			Node* newNode = new Node(std::forward<T>(element));
			AddNodeLast(newNode);
		}

		void AddLast(const T& element)
		{
			
			Node* newNode = new Node(element);
			AddNodeLast(newNode);
		}

		int Find(const T& element) const
		{
			
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

		void Insert(T&& element, size_t index)
		{
			
			Node* newNode = new Node(std::forward<T>(element));
			InsertNode(newNode, index);
		}

		void Insert(const T& element, size_t index)
		{
			
			Node* newNode = new Node(element);
			InsertNode(newNode, index);
		}

		void Remove(const T& element)
		{
			if (IsEmpty())
			{
				return;
			}

			Node* ptr1 = m_head;
			Node* ptr2 = m_head;

			while (ptr1 != m_dummy)
			{
				if (ptr1->element == element)
				{
					break;
				}

				ptr2 = ptr1;
				ptr1 = ptr1->next;
			}
		
			if (ptr1 == m_dummy)
			{
				return;
			}

			ptr2->next = ptr1->next;
			if (ptr1 == m_head)
			{
				m_head = ptr1->next;
			}
			delete ptr1;
			m_count--;
		}

		void RemoveAt(size_t index)
		{
			
			if(index == 0)
			{
				Node* ptr = m_head;
				m_head = m_head->next;
				delete ptr;
				m_count--;
				return;
			}

			Node* prev = GetNode(index - 1);
			RemoveNode(prev);
		}

		void Remove(ASinglyLinkedList<T>::AIterator iterator)
		{
			Remove(iterator.m_currNode->element);
		}
		
		void Clear() 
		{
			
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

		bool Contains(const T& element) const { return Find(element) != -1; }

		bool IsEmpty() const { return m_head == m_dummy; }

		ASinglyLinkedList::AIterator begin() { return ASinglyLinkedListIterator<T>(m_head); }

		ASinglyLinkedList::AIterator end() { return ASinglyLinkedListIterator<T>(m_dummy); }
		
		ASinglyLinkedList::AConstIterator begin() const { return ASinglyLinkedListConstIterator<T>(m_head); }
		
		ASinglyLinkedList::AConstIterator end() const { return ASinglyLinkedListConstIterator<T>(m_dummy); }

		T& operator[](size_t index) { return GetNode(index)->element; }
		
		const T& operator[](size_t index) const { return GetNode(index)->element; }

		ASinglyLinkedList<T>& operator=(const ASinglyLinkedList<T>& other)
		{
			Clear();

			for (const T& element : other)
			{
				Add(element);
			}
			return *this;
		}

		ASinglyLinkedList<T>& operator=(ASinglyLinkedList<T>&& other) noexcept
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
			Node(std::nullptr_t) : next(nullptr), element() { }
			Node(T&& e) : element(std::move(e)), next(nullptr) { }
			Node(const T& e) : element(e), next(nullptr) { }
			Node(const T& e, Node* ptr) : element(e), next(ptr) { }
		};

		void AddNode(Node* newNode) 
		{
			newNode->next = m_head;
			m_head = newNode;
			m_count++;
		}

		void AddNodeLast(Node* newNode)
		{
			Node* ptr = m_head;

			while (ptr->next != m_dummy)
			{
				ptr = ptr->next;
			}

			newNode->next = m_dummy;
			ptr->next = newNode;
			m_count++;
		}

		void InsertNode(Node* newNode, size_t index)
		{
			if (index == 0)
			{
				AddNode(newNode);
				return;
			}

			Node* prev = GetNode(index - 1);

			newNode->next = prev->next;
			prev->next = prev;
			m_count++;
		}

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
			
			m_currNode = m_currNode->next;
			return *this;
		}

		ASinglyLinkedListIterator<T>& operator+=(size_t i)
		{
			

			for (size_t j = 0; j < i; j++)
			{
				m_currNode = m_currNode->next;
			}

			return *this;
		}

		ASinglyLinkedListIterator<T> operator++(int)
		{
			
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
