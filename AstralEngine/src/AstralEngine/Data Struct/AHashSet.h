#pragma once
#include "ASinglyLinkedList.h"
#include "ADelegate.h"
#include "Math/AMath.h"

#define HASH_SET_MAX_LOAD_FACTOR 10

namespace AstralEngine
{
	template<typename T>
	class AHashSet;

	template<typename T>
	class AHashSetIterator
	{
		friend class AHashSet<T>;
		using Bucket = ASinglyLinkedList<T>;
		using BucketAIterator = ASinglyLinkedListIterator<T>;

	public:
		virtual ~AHashSetIterator() { }

		AHashSetIterator<T>& operator++()
		{
			m_currIt++;

			if (m_currIt == m_bucketArr[m_currBucket].end())
			{
				for (size_t i = m_currBucket + 1; i < m_maxCount; i++)
				{
					if (!m_bucketArr[i].IsEmpty())
					{
						m_currBucket = i;
						m_currIt = m_bucketArr[m_currBucket].begin();
						break;
					}
				}
			}

			return *this;
		}

		AHashSetIterator<T> operator++(int)
		{
			AHashSetIterator<T> it = *this;
			this->operator++();
			return it;
		}

		bool operator==(const AHashSetIterator<T>& other) const
		{
			return m_currIt == other.m_currIt;
		}

		bool operator!=(const AHashSetIterator<T>& other) const
		{
			return !(*this == other);
		}

		T& operator*()
		{
			return *m_currIt;
		}

	protected:
		AHashSetIterator(Bucket* bucketArr,
			size_t currBucket, BucketAIterator currIt, size_t maxCount)
			: m_bucketArr(bucketArr), m_currBucket(currBucket), m_currIt(currIt), m_maxCount(maxCount) { }

		AHashSetIterator(const AHashSetIterator<T>& other)
			: m_bucketArr(other.m_bucketArr), m_currBucket(other.m_currBucket),
			m_currIt(other.m_currIt), m_maxCount(other.m_maxCount) { }

	private:
		Bucket* m_bucketArr;
		size_t m_currBucket;
		size_t m_maxCount;
		BucketAIterator m_currIt;
	};

	template<typename T>
	class AHashSetConstIterator : public AHashSetIterator<T>
	{
		friend class AHashSet<T>;

		using Bucket = ASinglyLinkedList<T>;
		using BucketAIterator = ASinglyLinkedListIterator<T>;
	
	public:
		AHashSetConstIterator(const AHashSetConstIterator<T>& other)
			: AHashSetIterator<T>(other) { }
		
		AHashSetConstIterator<T>& operator++()
		{
			AHashSetIterator<T>::operator++();
			return *this;
		}

		AHashSetConstIterator<T> operator++(int)
		{
			AHashSetConstIterator<T> it = *this;
			this->operator++();
			return it;
		}

		bool operator==(const AHashSetConstIterator<T>& other) const
		{
			return AHashSetIterator<T>::operator==(other);
		}

		bool operator!=(const AHashSetConstIterator<T>& other) const
		{
			return !(*this == other);
		}

		const T& operator*() const
		{
			return ((AHashSetIterator*)(this))->AHashSetIterator<T>::operator*();
		}

	private:
		AHashSetConstIterator(Bucket* bucketArr,
			size_t currBucket, BucketAIterator currIt, size_t maxCount)
			: AHashSetIterator<T>(bucketArr, currBucket, currIt, maxCount) { }
	};

	template<typename T>
	class AHashSet
	{
	private:
		using Bucket = ASinglyLinkedList<T>;

	public:
		using AIterator = AHashSetIterator<T>;
		using AConstIterator = AHashSetConstIterator<T>;

		AHashSet(size_t bucketCount = 5, std::function<bool(const T&, const T&)> equalsFunc
			= &AHashSet<T>::DefaultEquals, std::function<int(long, size_t)> compressFunc
			= &AHashSet<T>::DefaultCompress) : m_bucketCount(bucketCount), m_count(0), 
			m_equalsFunc(equalsFunc), m_compressFunc(compressFunc), m_bucketArr(new Bucket[m_bucketCount]) { }

		AHashSet(std::initializer_list<T> list) : m_bucketCount(5), m_count(0),
			m_equalsFunc(&AHashSet<T>::DefaultEquals), m_compressFunc(&AHashSet<T>::DefaultCompress), 
			m_bucketArr(new Bucket[m_bucketCount])
		{
			for (const T& e : list)
			{
				Add(e);
			}
		}

		AHashSet(const AHashSet<T>& other) : m_bucketCount(other.m_bucketCount), m_count(other.m_count), 
			m_equalsFunc(other.m_equalsFunc), m_compressFunc(other.m_compressFunc), m_bucketArr(new Bucket[m_bucketCount])
		{
			for (const T& e : other)
			{
				AddWithoutUpdate(e);
			}
		}

		AHashSet(AHashSet<T>&& other) : m_bucketCount(other.m_bucketCount), m_count(other.m_count),
			m_equalsFunc(other.m_equalsFunc), m_compressFunc(other.m_compressFunc), m_bucketArr(other.m_bucketArr)
		{
			other.m_bucketArr = nullptr;
		}

		~AHashSet()
		{
			delete[] m_bucketArr;
		}

		size_t GetCount() const { return m_count; }

		void Add(const T& e)
		{
			if (!Contains(e))
			{
				AddWithoutContainCheck(e);
			}
		}

		AHashSet<T> Union(const AHashSet<T>& other) const
		{
			AHashSet<T> u = *this;

			for (const T& e : other)
			{
				u.Add(e);
			}
			return u;
		}

		AHashSet<T> Intersection(const AHashSet<T>& other) const
		{
			AHashSet<T> u;

			for (const T& e : other)
			{
				if (Contains(e))
				{
					u.AddWithoutContainCheck(e);
				}
			}
			return u;
		}

		bool IsSubsetOf(const AHashSet<T>& other) const
		{
			for (const T& e : *this)
			{
				if (!other.Contains(e))
				{
					return false;
				}
			}
			return true;
		}

		bool IsProperSubsetOf(const AHashSet<T>& other) const
		{
			return !IsEmpty() && GetCount() < other.GetCount() && IsSubsetOf(other);
		}

		void Remove(const T& e)
		{
			Bucket b = m_bucketArr[GetBucketIndex(e)];
			for (Bucket::AIterator it = b.begin(); it != b.end(); it++)
			{
				if (m_equalsFunc(*it, e))
				{
					b.Remove(it);
					m_count--;
					break;
				}
			}
		}

		bool Contains(const T& e) const
		{
			return m_bucketArr[GetBucketIndex(e)].Contains(e);
		}

		bool IsEmpty() const { return m_count == 0; }

		void Clear()
		{
			for (size_t i = 0; i < m_bucketCount; i++)
			{
				m_bucketArr[i].Clear();
			}

			m_count = 0;
		}

		AIterator begin()
		{
			size_t bucketIndex;
			for (size_t i = 0; i < m_bucketCount; i++)
			{
				if (!m_bucketArr[i].IsEmpty())
				{
					bucketIndex = i;
					break;
				}
			}

			return AHashSetIterator<T>(m_bucketArr, bucketIndex, m_bucketArr[bucketIndex].begin(), m_bucketCount);
		}

		AIterator end()
		{
			size_t bucketIndex;
			for (size_t i = m_bucketCount - 1; i >= 0; i--)
			{
				if (!m_bucketArr[i].IsEmpty())
				{
					bucketIndex = i;
					break;
				}
			}
			return AHashSetIterator<T>(m_bucketArr, bucketIndex, m_bucketArr[bucketIndex].end(), m_bucketCount);
		}

		AConstIterator begin() const
		{
			size_t bucketIndex;
			for (size_t i = 0; i < m_bucketCount; i++)
			{
				if (!m_bucketArr[i].IsEmpty())
				{
					bucketIndex = i;
					break;
				}
			}
			return AHashSetConstIterator<T>(m_bucketArr, bucketIndex, m_bucketArr[bucketIndex].begin(), m_bucketCount);
		}

		AConstIterator end() const
		{
			size_t bucketIndex;
			for (size_t i = m_bucketCount - 1; i >= 0; i--)
			{
				if (!m_bucketArr[i].IsEmpty())
				{
					bucketIndex = i;
					break;
				}
			}

			return AHashSetConstIterator<T>(m_bucketArr, bucketIndex, m_bucketArr[bucketIndex].end(), m_bucketCount);
		}

		AHashSet<T>& operator=(const AHashSet<T>& other)
		{
			delete m_bucketArr;

			m_bucketArr = new Bucket[other.m_bucketCount];
			m_count = other.m_count;
			m_equalsFunc = other.m_equalsFunc;
			m_compressFunc = other.m_compressFunc;
			m_bucketCount = other.m_bucketCount;

			for (T& e : other)
			{
				AddWithoutUpdate(e);
			}

			return *this;
		}

		AHashSet<T>& operator=(AHashSet<T>&& other)
		{
			delete m_bucketArr;

			m_bucketArr = other.m_bucketArr;
			other.m_bucketArr = nullptr;
			m_count = other.m_count;
			m_equalsFunc = other.m_equalsFunc;
			m_compressFunc = other.m_compressFunc;
			m_bucketCount = other.m_bucketCount;

			return *this;
		}

		bool operator==(const AHashSet<T>& other) const
		{
			if (GetCount() != other.GetCount())
			{
				return false;
			}

			for (const T& e : *this)
			{
				if (!other.Contains(e))
				{
					return false;
				}
			}

			return true;
		}

		bool operator!=(const AHashSet<T>& other) const
		{
			return !(*this == other);
		}

	private:
		static bool DefaultEquals(const T& e1, const T& e2)
		{
			return e1 == e2;
		}

		static int DefaultCompress(long hash, size_t size)
		{
			return (7l * hash + 31l) % size;
		}

		int GetBucketIndex(const T& e) const
		{
			return m_compressFunc((int)Math::Abs(m_hash(e)), m_bucketCount);
		}

		//only adds the element in the set without updating count or checking the load factor
		void AddWithoutUpdate(const T& e)
		{
			m_bucketArr[GetBucketIndex(e)].Add(e);
		}

		//adds but does not check if set already contains element
		void AddWithoutContainCheck(const T& e)
		{
			AddWithoutUpdate(e);
			m_count++;

			if (GetCount() / m_bucketCount >= HASH_SET_MAX_LOAD_FACTOR)
			{
				Rehash();
			}
		}

		void Rehash()
		{
			size_t numBuckets = (size_t)((float)m_bucketCount * 1.5f);

			if (numBuckets == m_bucketCount)
			{
				numBuckets += 5;
			}

			size_t oldBucketCount = m_bucketCount;
			Bucket* oldBuckets = m_bucketArr;
			m_bucketArr = new Bucket[numBuckets];
			m_count = 0;
			m_bucketCount = numBuckets;

			for (size_t i = 0; i < oldBucketCount; i++)
			{
				for (T& e : oldBuckets[i])
				{
					Add(e);
				}
			}

			delete[] oldBuckets;
		}

		ADelegate<bool(const T&, const T&)> m_equalsFunc;
		ADelegate<int(long, size_t)> m_compressFunc;
		std::hash<T> m_hash;
		size_t m_bucketCount;
		size_t m_count;
		ASinglyLinkedList<T>* m_bucketArr;
	};
}