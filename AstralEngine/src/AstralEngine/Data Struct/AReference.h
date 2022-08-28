#pragma once

namespace AstralEngine
{
	template<typename T>
	class AWeakPtr;

	struct ControlBlock
	{
	private:
		template<typename T>
		friend class AReference;
		template<typename T>
		friend class AWeakRef;
		int count;
		int weakCount;
		void* ptr;
		ControlBlock(void* p) : ptr(p), count(1), weakCount(0) { }

		void DecrementWeak()
		{
			weakCount--;
			DecrementCheck();
		}

		void DecrementStrong()
		{
			count--;
			DecrementCheck();
		}

		void DecrementCheck()
		{
			if (count == 0)
			{
				if (weakCount == 0)
				{
					delete this;
				}
				else
				{
					delete ptr;
					ptr = nullptr;
				}
			}
		}
	};


	template<typename T>
	class AReference
	{
		template<typename Other>
		friend class AReference;

		template<typename T>
		friend class AWeakRef;

	public:
		AReference() : m_block(nullptr) { }

		AReference(std::nullptr_t) : m_block(nullptr) {	}

		template<typename Derived>
		AReference(const AReference<Derived>& other) : m_block(other.m_block)
		{
			if (m_block != nullptr)
			{
				m_block->count++;
			}
		}
		
		AReference(const AReference<T>& other) : m_block(other.m_block)
		{
			if (other.m_block != nullptr)
			{
				m_block->count++;
			}
		}

		AReference(const AWeakRef<T>& other) : m_block(other.m_block)
		{
			if (other.m_block != nullptr)
			{
				m_block->count++;
			}
		}

		~AReference()
		{
			if (m_block != nullptr)
			{
				m_block->DecrementStrong();
			}
		}

		T* Get() const { return (T*)m_block->ptr; }

		T* operator->() const
		{
			return (T*)m_block->ptr;
		}

		T& operator*() const
		{
			return *((T*)m_block->ptr);
		}

		void operator=(const AReference<T>& other)
		{
			if (m_block != nullptr)
			{
				m_block->DecrementStrong();
			}

			m_block = other.m_block;
			
			if (m_block != nullptr)
			{
				m_block->count++;
			}
		}

		void operator=(const AWeakRef<T>& other)
		{
			if (m_block != nullptr)
			{
				m_block->DecrementStrong();
			}
			m_block = other.m_block;

			if (m_block != nullptr)
			{
				m_block->count++;
			}
		}

		bool operator==(std::nullptr_t n) const
		{
			if (m_block == nullptr)
			{
				return true;
			}

			return ((T*)m_block->ptr) == nullptr;
		}

		bool operator!=(std::nullptr_t n) const
		{
			return !(*this == n);
		}

		bool operator==(const AReference<T>& other) const
		{
			if (m_block == nullptr)
			{
				return other.m_block == nullptr;
			}

			return *((T*)m_block->ptr) == *((T*)other.m_block->ptr);
		}

		bool operator==(const AWeakRef<T>& other) const
		{
			if (m_block == nullptr)
			{
				return other.m_block == nullptr;
			}

			return *((T*)m_block->ptr) == *((T*)other.m_block->ptr);
		}

		bool operator!=(const AReference<T>& other) const
		{
			
			return !(*this == other);
		}

		bool operator!=(const AWeakRef<T>& other) const
		{
			
			return !(*this == other);
		}

		template<typename... Args>
		static AReference<T> Create(Args... args)
		{
			AReference<T> ref;
			T* ptr = new T(std::forward<Args>(args)...);
			ref.m_block = new ControlBlock(ptr);
			return ref;
		}

	private:

		ControlBlock* m_block;
	};

	template<typename...>
	struct RemoveRef;

	template<typename T>
	struct RemoveRef<T>
	{
		using type = T;
	};

	template<typename T>
	struct RemoveRef<AReference<T>>
	{
		using type = T;
	};

	template<typename T>
	using RemoveRef_t = typename RemoveRef<T>::type;

}