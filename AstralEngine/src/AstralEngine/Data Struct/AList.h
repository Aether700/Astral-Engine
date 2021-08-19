#pragma once

namespace AstralEngine
{
	template<typename T>
	class AList
	{
	public:
		virtual ~AList() { }
		virtual bool IsEmpty() const { return GetCount() == 0; }
		virtual size_t GetCount() const = 0;
		virtual void Add(T element) = 0;
		virtual void AddFirst(T element) = 0;
		virtual void AddLast(T element) = 0;
		virtual size_t Find(const T& element) const = 0;
		virtual void Insert(T element, size_t index) = 0;
		virtual void Remove(T element) = 0;
		virtual void RemoveAt(size_t index) = 0;
		virtual void Reserve(size_t count) = 0;
		virtual void Clear() = 0;
		virtual T& Get(size_t index) { return (*this)[index]; }
		virtual const T& Get(size_t index) const { return (*this)[index]; }
		virtual bool Contains(const T& element) const = 0;
		virtual T& operator[](size_t index) = 0;
		virtual const T& operator[](size_t index) const = 0;
	};
}