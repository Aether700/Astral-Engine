#pragma once

namespace AstralEngine
{
	template<typename...>
	class AUniqueRef;

	template<typename T, typename DeleteFunc>
	class AUniqueRef<T, DeleteFunc>;

	template<typename T>
	class AUniqueRef<T>
	{
		template<typename...>
		friend class AUniqueRef;

	public:
		AUniqueRef() : m_ptr(nullptr) { }

		AUniqueRef(std::nullptr_t null) : m_ptr(nullptr) { }
		
		AUniqueRef(AUniqueRef<T>& other) : m_ptr(other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		template<typename Other>
		AUniqueRef(AUniqueRef<Other>& other) : m_ptr((T*)other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		template<typename Other, typename Del>
		AUniqueRef(AUniqueRef<Other, Del>& other) : m_ptr((T*)other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		~AUniqueRef() { delete m_ptr; }

		T* const Get() const { return m_ptr; }

		T* operator->() const
		{
			return m_ptr;
		}

		T& operator*() const
		{
			return *m_ptr;
		}

		operator bool() const
		{
			return m_ptr != nullptr;
		}

		AUniqueRef<T>& operator=(std::nullptr_t)
		{
			delete m_ptr;
			m_ptr = nullptr;

			return *this;
		}

		AUniqueRef<T>& operator=(AUniqueRef<T>& other)
		{
			delete m_ptr;
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;

			return *this;
		}

		template<typename Other>
		AUniqueRef<T>& operator=(AUniqueRef<Other>& other)
		{
			delete m_ptr;
			m_ptr = (T*) other.m_ptr;
			other.m_ptr = nullptr;

			return *this;
		}

		template<typename Other, typename Del>
		AUniqueRef<T>& operator=(AUniqueRef<Other, Del>& other)
		{
			delete m_ptr;
			m_ptr = (T*)other.m_ptr;
			other.m_ptr = nullptr;

			return *this;
		}

		bool operator==(const AUniqueRef<T>& other) const
		{
			if (m_ptr == nullptr)
			{
				return other.m_ptr == nullptr;
			}
			else if (other.m_ptr == nullptr)
			{
				return false;
			}

			return *m_ptr == *other.m_ptr;
		}

		bool operator!=(const AUniqueRef<T>& other) const
		{
			return !(*this == other);
		}

		bool operator==(std::nullptr_t) const
		{
			return m_ptr == nullptr;
		}

		bool operator!=(std::nullptr_t n) const
		{
			return !(*this == n);
		}

		template<typename... Args>
		static AUniqueRef<T> Create(Args&&... args)
		{
			AUniqueRef<T> uniqueRef;
			uniqueRef.m_ptr = new T(std::forward<Args>(args)...);
			return uniqueRef;
		}

	private:
		T* m_ptr;
	};

	template<typename T>
	class AUniqueRef<T[]>
	{
	public:
		AUniqueRef() : m_ptr(nullptr) { }
		
		AUniqueRef(std::nullptr_t null) : m_ptr(nullptr) { }

		AUniqueRef(AUniqueRef<T[]>& other) : m_ptr(other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		~AUniqueRef() { delete[] m_ptr; }

		T* const Get() const { return m_ptr; }

		T& operator[](size_t index) const
		{
			return m_ptr[index];
		}

		operator bool() const
		{
			return m_ptr != nullptr;
		}

		AUniqueRef<T[]>& operator=(std::nullptr_t)
		{
			delete[] m_ptr;
			m_ptr = nullptr;

			return *this;
		}

		AUniqueRef<T[]>& operator=(AUniqueRef<T[]>& other)
		{
			delete[] m_ptr;
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;

			return *this;
		}

		bool operator==(const AUniqueRef<T[]>& other) const
		{
			return m_ptr == other.m_ptr;
		}

		bool operator!=(const AUniqueRef<T[]>& other) const
		{
			return !(*this == other);
		}

		bool operator==(std::nullptr_t) const
		{
			return m_ptr == nullptr;
		}

		bool operator!=(std::nullptr_t n) const
		{
			return !(*this == n);
		}

		static AUniqueRef<T[]> Create(size_t size)
		{
			AUniqueRef<T[]> uniqueRef;
			uniqueRef.m_ptr = new T[size];
			return uniqueRef;
		}

	private:
		T* m_ptr;
	};

	template<>
	class AUniqueRef<void>
	{
		template<typename...>
		friend class AUniqueRef;

	public:
		AUniqueRef() : m_ptr(nullptr) { }

		AUniqueRef(std::nullptr_t null) : m_ptr(nullptr) { }


		AUniqueRef(AUniqueRef<void>& other) : m_ptr(other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		template<typename Other>
		AUniqueRef(AUniqueRef<Other>& other) : m_ptr((void*)other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		template<typename Other, typename Del>
		AUniqueRef(AUniqueRef<Other, Del>& other) : m_ptr((void*)other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		~AUniqueRef() { delete m_ptr; }

		void* const Get() const { return m_ptr; }

		void* operator->() const
		{
			return m_ptr;
		}

		operator bool() const
		{
			return m_ptr != nullptr;
		}

		AUniqueRef<void>& operator=(std::nullptr_t)
		{
			delete m_ptr;
			m_ptr = nullptr;

			return *this;
		}

		AUniqueRef<void>& operator=(AUniqueRef<void>& other)
		{
			delete m_ptr;
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;

			return *this;
		}

		template<typename Other>
		AUniqueRef<void>& operator=(AUniqueRef<Other>& other)
		{
			delete m_ptr;
			m_ptr = (T*)other.m_ptr;
			other.m_ptr = nullptr;

			return *this;
		}

		template<typename Other, typename Del>
		AUniqueRef<void>& operator=(AUniqueRef<Other, Del>& other)
		{
			delete m_ptr;
			m_ptr = (T*)other.m_ptr;
			other.m_ptr = nullptr;

			return *this;
		}

		bool operator==(const AUniqueRef<void>& other) const
		{
			return m_ptr == other.m_ptr;
		}

		bool operator!=(const AUniqueRef<void>& other) const
		{
			return !(*this == other);
		}

		bool operator==(std::nullptr_t) const
		{
			return m_ptr == nullptr;
		}

		bool operator!=(std::nullptr_t n) const
		{
			return !(*this == n);
		}

	private:
		void* m_ptr;
	};


	template<typename T>
	class AUniqueRef<T, void(T*)>
	{
		template<typename...>
		friend class AUniqueRef;

		using DeleteFunc = ADelegate<void(T*)>;

	public:
		AUniqueRef() : m_ptr(nullptr), m_del(&DefaultDelete) { }

		AUniqueRef(std::nullptr_t null) : m_ptr(nullptr) { }

		AUniqueRef(T* ptr,  DeleteFunc del) : m_ptr(ptr), m_del(del) { }

		AUniqueRef(AUniqueRef<T>& other) : m_ptr(other.m_ptr), m_del(&DefaultDelete)
		{
			other.m_ptr = nullptr;
		}

		template<typename Other>
		AUniqueRef(AUniqueRef<Other>& other) : m_ptr((T*)other.m_ptr), m_del(DefaultDelete)
		{
			other.m_ptr = nullptr;
		}

		template<typename Other, typename Del>
		AUniqueRef(AUniqueRef<Other, Del>& other, DeleteFunc del = &DefaultDelete) 
			: m_ptr((T*)other.m_ptr), m_del(&del)
		{
			other.m_ptr = nullptr;
			other.m_del = nullptr;
		}

		AUniqueRef(AUniqueRef<T, void(T*)>& other) : m_ptr(other.m_ptr), m_del(other.m_del)
		{
			other.m_ptr = nullptr;
			other.m_del = nullptr;
		}

		~AUniqueRef() 
		{
			if (m_ptr != nullptr)
			{
				(*m_del)(m_ptr);
			}
		}

		T* const Get() const { return m_ptr; }

		T* operator->() const
		{
			return m_ptr;
		}

		operator bool() const
		{
			return m_ptr != nullptr;
		}

		AUniqueRef<T, void(T*)>& operator=(std::nullptr_t)
		{
			delete m_ptr;
			m_ptr = nullptr;

			return *this;
		}

		AUniqueRef<T, void(T*)>& operator=(AUniqueRef<T, void(T*)>& other)
		{
			delete m_ptr;
			m_ptr = other.m_ptr;
			m_del = other.m_del;

			other.m_ptr = nullptr;
			other.m_del = nullptr;

			return *this;
		}

		template<typename Other>
		AUniqueRef<T, void(T*)>& operator=(AUniqueRef<Other>& other)
		{
			delete m_ptr;
			m_ptr = (T*)other.m_ptr;
			other.m_ptr = nullptr;
			m_del = DefaultDelete;

			return *this;
		}

		template<typename Other, typename Del>
		AUniqueRef<T, void(T*)>& operator=(AUniqueRef<Other, Del>& other)
		{
			delete m_ptr;
			m_ptr = (T*)other.m_ptr;
			m_del = [](T* p) { other.m_del((Other*)p); };

			other.m_ptr = nullptr;
			other.m_del = nullptr;

			return *this;
		}

		bool operator==(const AUniqueRef<T, void(T*)>& other) const
		{
			if (m_ptr == nullptr)
			{
				return other.m_ptr == nullptr;
			}
			else if (other.m_ptr == nullptr)
			{
				return false;
			}

			return m_ptr == other.m_ptr;
		}

		bool operator!=(const AUniqueRef<T, void(T*)>& other) const
		{
			return !(*this == other);
		}

		bool operator==(const AUniqueRef<T>& other) const
		{
			if (m_ptr == nullptr)
			{
				return other.m_ptr == nullptr;
			}
			else if (other.m_ptr == nullptr)
			{
				return false;
			}

			return *m_ptr == *other.m_ptr;
		}

		bool operator!=(const AUniqueRef<T>& other) const
		{
			return !(*this == other);
		}

		bool operator==(std::nullptr_t) const
		{
			return m_ptr == nullptr;
		}

		bool operator!=(std::nullptr_t n) const
		{
			return !(*this == n);
		}

	private:

		static void DefaultDelete(T* ptr)
		{
			delete ptr;
		}

		T* m_ptr;
		DeleteFunc m_del;
	};

	template<>
	class AUniqueRef<void, void(void*)>
	{
		template<typename...>
		friend class AUniqueRef;
		using DeleteFunc = void(void*);

	public:
		AUniqueRef() : m_ptr(nullptr), m_del(DefaultDelete) { }

		AUniqueRef(std::nullptr_t null) : m_ptr(nullptr), m_del(DefaultDelete) { }

		AUniqueRef(void* ptr, DeleteFunc* del) : m_ptr(ptr), m_del(del) { }

		AUniqueRef(AUniqueRef<void>& other) : m_ptr(other.m_ptr), m_del(DefaultDelete)
		{
			other.m_ptr = nullptr;
		}

		template<typename Other>
		AUniqueRef(AUniqueRef<Other>& other) : m_ptr((T*)other.m_ptr), m_del(DefaultDelete)
		{
			other.m_ptr = nullptr;
		}

		template<typename Other, typename Del>
		AUniqueRef(AUniqueRef<Other, Del>& other) : m_ptr((T*)other.m_ptr), m_del([other](T* p) { other.m_del((Other*)p); })
		{
			other.m_ptr = nullptr;
			other.m_del = nullptr;
		}

		AUniqueRef(AUniqueRef<void, DeleteFunc>& other) : m_ptr(other.m_ptr), m_del(other.m_del)
		{
			other.m_ptr = nullptr;
			other.m_del = nullptr;
		}

		~AUniqueRef()
		{
			if (m_ptr != nullptr)
			{
				(*m_del)(m_ptr);
			}
		}

		void* const Get() const { return m_ptr; }

		void* operator->() const
		{
			return m_ptr;
		}

		operator bool() const
		{
			return m_ptr != nullptr;
		}

		AUniqueRef<void, DeleteFunc>& operator=(std::nullptr_t)
		{
			delete m_ptr;
			m_ptr = nullptr;

			return *this;
		}

		AUniqueRef<void, DeleteFunc>& operator=(AUniqueRef<void, DeleteFunc>& other)
		{
			delete m_ptr;
			m_ptr = other.m_ptr;
			m_del = other.m_del;

			other.m_ptr = nullptr;
			other.m_del = nullptr;

			return *this;
		}

		template<typename Other>
		AUniqueRef<void, DeleteFunc>& operator=(AUniqueRef<Other>& other)
		{
			delete m_ptr;
			m_ptr = (void*)other.m_ptr;
			other.m_ptr = nullptr;
			m_del = DefaultDelete;

			return *this;
		}

		template<typename Other, typename Del>
		AUniqueRef<void, DeleteFunc>& operator=(AUniqueRef<Other, Del>& other)
		{
			delete m_ptr;
			m_ptr = (void*)other.m_ptr;
			m_del = [](void* p) { other.m_del((Other*)p); };

			other.m_ptr = nullptr;
			other.m_del = nullptr;

			return *this;
		}

		bool operator==(const AUniqueRef<void, DeleteFunc>& other) const
		{
			if (m_ptr == nullptr)
			{
				return other.m_ptr == nullptr;
			}
			else if (other.m_ptr == nullptr)
			{
				return false;
			}

			return m_ptr == other.m_ptr;
		}

		bool operator!=(const AUniqueRef<void, DeleteFunc>& other) const
		{
			return !(*this == other);
		}

		bool operator==(const AUniqueRef<void>& other) const
		{
			if (m_ptr == nullptr)
			{
				return other.m_ptr == nullptr;
			}
			else if (other.m_ptr == nullptr)
			{
				return false;
			}

			return m_ptr == other.m_ptr;
		}

		bool operator!=(const AUniqueRef<void>& other) const
		{
			return !(*this == other);
		}

		bool operator==(std::nullptr_t) const
		{
			return m_ptr == nullptr;
		}

		bool operator!=(std::nullptr_t n) const
		{
			return !(*this == n);
		}

	private:

		static void DefaultDelete(void* ptr)
		{
			delete ptr;
		}

		void* m_ptr;
		DeleteFunc* m_del;
	};


}