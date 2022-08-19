#pragma once
#include "AstralEngine/Core/Core.h"
#include "AstralEngine/Math/AMath.h"
#include "ASinglyLinkedList.h"
#include "AKeyElementPair.h"
#include <functional>

#define UNORDERED_MAP_MAX_LOAD_FACTOR 10

namespace AstralEngine
{
	template<typename K, typename T>
	class AUnorderedMap;

	template<typename K, typename T>
	class AUnorderedMapConstIterator;

	template<typename K, typename T>
	class AUnorderedMapIterator
	{
		friend class AUnorderedMap<K, T>;
		friend class AUnorderedMapConstIterator<K, T>;
		
		using Bucket = ASinglyLinkedList<AKeyElementPair<K, T>>;
		using BucketAIterator = ASinglyLinkedListIterator<AKeyElementPair<K, T>>;
	public:
		AUnorderedMapIterator(const AUnorderedMapIterator<K, T>& other)
			: m_bucketArr(other.m_bucketArr), m_currBucket(other.m_currBucket),
			m_currIt(other.m_currIt), m_maxCount(other.m_maxCount) { }

		virtual ~AUnorderedMapIterator() { }

		AUnorderedMapIterator<K, T>& operator++()
		{
			AE_PROFILE_FUNCTION();
			
			m_currIt++;

			if (m_currIt == m_bucketArr[m_currBucket].end())
			{
				for (size_t i = m_currBucket + 1; i < m_maxCount; i++)
				{
					if (m_bucketArr[i].GetCount() != 0)
					{
						m_currBucket = i;
						m_currIt = m_bucketArr[m_currBucket].begin();
						break;
					}
				}
			}

			return *this;
		}

		AUnorderedMapIterator<K, T> operator++(int)
		{
			AUnorderedMapIterator<K, T> it = *this;
			this->operator++();
			return it;
		}

		bool operator==(const AUnorderedMapIterator<K, T>& other) const
		{
			return m_currIt == other.m_currIt;
		}

		bool operator!=(const AUnorderedMapIterator<K, T>& other) const
		{
			return !(*this == other);
		}

		AKeyElementPair<K, T>& operator*()
		{
			return *m_currIt;
		}

	private:
		AUnorderedMapIterator(Bucket* bucketArr,
			size_t currBucket, BucketAIterator& currIt, size_t maxCount)
			: m_bucketArr(bucketArr), m_currBucket(currBucket), m_currIt(currIt), m_maxCount(maxCount) { }

		Bucket* m_bucketArr;
		size_t m_currBucket;
		size_t m_maxCount;
		BucketAIterator m_currIt;
	};

	template<typename K, typename T>
	class AUnorderedMapConstIterator : public AUnorderedMapIterator<K, T>
	{
		friend class AUnorderedMap<K, T>;
	public:
		AUnorderedMapConstIterator(const AUnorderedMapConstIterator<K, T>& other)
			: AUnorderedMapIterator<K, T>(other) { }

		AUnorderedMapConstIterator<K, T>& operator++()
		{
			AUnorderedMapIterator<K, T>::operator++();
			return *this;
		}

		AUnorderedMapConstIterator<K, T> operator++(int)
		{
			AUnorderedMapConstIterator<K, T> it = *this;
			this->operator++();
			return it;
		}

		AUnorderedMapConstIterator<K, T>& operator--()
		{
			AUnorderedMapIterator<K, T>::operator--();
			return *this;
		}

		AUnorderedMapConstIterator<K, T> operator--(int)
		{
			AE_PROFILE_FUNCTION();
			AUnorderedMapConstIterator<K, T> it = *this;
			this->operator--();
			return it;
		}

		bool operator==(const AUnorderedMapConstIterator<K, T>& other) const
		{
			return AUnorderedMapIterator<K, T>::operator==(other);
		}

		bool operator!=(const AUnorderedMapConstIterator<K, T>& other) const
		{
			return !(*this == other);
		}

		const AKeyElementPair<K, T>& operator*() const
		{
			return AUnorderedMapIterator<K, T>::operator*();
		}

	private:
		AUnorderedMapConstIterator(ASinglyLinkedList<AKeyElementPair<K, T>>* bucketArr,
			size_t currBucket, ASinglyLinkedListIterator<AKeyElementPair<K, T>>& currIt, size_t numBuckets)
			: AUnorderedMapIterator<K, T>(bucketArr, currBucket, currIt, numBuckets) { }

	};

	template<typename K, typename T>
	class AUnorderedMap
	{
	public:
		using AIterator = AUnorderedMapIterator<K, T>;
		using AConstIterator = AUnorderedMapConstIterator<K, T>;

		AUnorderedMap(size_t bucketCount = 5, std::function<bool(const K&, const K&)> equalsFunc
			= &AUnorderedMap<K, T>::DefaultEquals, std::function<int(long, size_t)> compressFunc
			= &AUnorderedMap<K, T>::DefaultCompress)
			: m_bucketCount(bucketCount), m_equalsFunc(equalsFunc), m_compressFunc(compressFunc), m_count(0)
		{
			m_bucketArr = new ASinglyLinkedList<AKeyElementPair<K, T>>[m_bucketCount];
		}

		AUnorderedMap(std::function<bool(const K&, const K&)> equalsFunc) : m_bucketCount(5),
			m_equalsFunc(equalsFunc), m_compressFunc(&AUnorderedMap<K, T>::DefaultCompress), m_count(0)
		{
			m_bucketArr = new ASinglyLinkedList<AKeyElementPair<K, T>>[m_bucketCount];
		}

		AUnorderedMap(std::function<int(long, size_t)> compressFunc) : m_bucketCount(5),
			m_equalsFunc(&AUnorderedMap<K, T>::DefaultEquals), m_compressFunc(compressFunc), m_count(0)
		{
			m_bucketArr = new ASinglyLinkedList<AKeyElementPair<K, T>>[m_bucketCount];
		}

		AUnorderedMap(const AUnorderedMap<K, T>& other) : m_bucketCount(other.m_bucketCount), m_count(other.m_count),
			m_equalsFunc(other.m_equalsFunc), m_compressFunc(other.m_compressFunc)
		{
			m_bucketArr = new ASinglyLinkedList<AKeyElementPair<K, T>>[m_bucketCount];
			for (size_t i = 0; i < m_bucketCount; i++)
			{
				m_bucketArr[i] = other.m_bucketArr[i];
			}
		}

		AUnorderedMap(AUnorderedMap<K, T>&& other) noexcept : m_bucketCount(other.m_bucketCount),
			m_equalsFunc(other.m_equalsFunc), m_compressFunc(other.m_compressFunc), m_count(other.m_count)
		{
			AE_PROFILE_FUNCTION();
			m_bucketArr = other.m_bucketArr;

			other.m_bucketArr = nullptr;
			other.m_bucketCount = 0;
			other.m_count = 0;
		}

		~AUnorderedMap()
		{
			delete[] m_bucketArr;
		}


		size_t GetCount() const
		{
			return m_count;
		}

		void Add(const K& key, const T& element)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(!ContainsKey(key), "Key Already contained");
			size_t bucketIndex = GetBucketIndex(key);

			AKeyElementPair<K, T> pair = AKeyElementPair<K, T>(key, element);
			m_bucketArr[bucketIndex].Add(pair);
			m_count++;
			CheckLoadFactor();
		}

		void Set(const K& key, const T& element)
		{
			this->operator[](key) = element;
		}

		void Remove(const K& key)
		{
			AE_PROFILE_FUNCTION();
			if (!ContainsKey(key))
			{
				return;
			}

			size_t bucketIndex = GetBucketIndex(key);
			
			for (auto it = m_bucketArr[bucketIndex].begin(); it != m_bucketArr[bucketIndex].end(); it++)
			{
				if (m_equalsFunc(key, (*it).GetKey()))
				{
					m_bucketArr[bucketIndex].Remove(it);
				}
			}
		}

		T& Get(const K& key)
		{
			return this->operator[](key);
		}

		const T& Get(const K& key) const
		{
			return this->operator[](key);
		}

		bool ContainsKey(const K& key) const
		{
			int bucketIndex = GetBucketIndex(key);
			for (AKeyElementPair<K, T> pair : m_bucketArr[bucketIndex])
			{
				if (m_equalsFunc(pair.GetKey(), key))
				{
					return true;
				}
			}
			return false;
		}

		void Clear()
		{
			AE_PROFILE_FUNCTION();
			for (size_t i = 0; i < m_bucketCount; i++)
			{
				m_bucketArr[i].Clear();
			}
		}

		T& operator[](const K& key)
		{
			AE_PROFILE_FUNCTION();
			size_t bucketIndex = GetBucketIndex(key);

			//try to find existing key
			for (AKeyElementPair<K, T>& pair : m_bucketArr[bucketIndex])
			{
				if (m_equalsFunc(pair.GetKey(), key))
				{
					return pair.GetElement();
				}
			}

			//if no existing key was created found create one and add it to the AUnorderedMap
			AKeyElementPair<K, T> pair = AKeyElementPair<K, T>(key);
			m_bucketArr[bucketIndex].Add(pair);
			m_count++;

			if (CheckLoadFactor())
			{
				bucketIndex = GetBucketIndex(key);
			}

			//find the newly added key and return it's element
			for (AKeyElementPair<K, T>& pair : m_bucketArr[bucketIndex])
			{
				if (pair.GetKey() == key)
				{
					return pair.GetElement();
				}
			}

			AE_CORE_ERROR("AUnorderedMap could not find new added key");
			return pair.GetElement();
		}

		const T& operator[](const K& key) const
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(ContainsKey(key), "AUnorderedMap could not find the provided key");
			size_t bucketIndex = GetBucketIndex(key);

			for (AKeyElementPair<K, T>& pair : m_bucketArr[bucketIndex])
			{
				if (m_equalsFunc(pair.GetKey(), key))
				{
					return pair.GetElement();
				}
			}

			return m_bucketArr[bucketIndex][0].GetElement();
		}

		AUnorderedMap<K, T>::AIterator begin()
		{
			size_t bucketIndex = m_bucketCount - 1;
			for (size_t i = 0; i < m_bucketCount; i++)
			{
				if (!m_bucketArr[i].IsEmpty())
				{
					bucketIndex = i;
					break;
				}
			}

			return AUnorderedMapIterator<K, T>(m_bucketArr, bucketIndex, m_bucketArr[bucketIndex].begin(), m_bucketCount);
		}

		AUnorderedMap<K, T>::AIterator end()
		{
			size_t bucketIndex = m_bucketCount - 1;
			for (int i = m_bucketCount - 1; i >= 0; i--)
			{
				if (!m_bucketArr[i].IsEmpty())
				{
					bucketIndex = (size_t)i;
					break;
				}
			}
			return AUnorderedMapIterator<K, T>(m_bucketArr, bucketIndex, m_bucketArr[bucketIndex].end(), m_bucketCount);
		}

		AUnorderedMap<K, T>::AConstIterator begin() const
		{
			size_t bucketIndex;
			for (size_t i = 0; i < m_bucketCount; i++)
			{
				if (m_bucketArr[i].GetCount() != 0)
				{
					bucketIndex = i;
					break;
				}
			}
			return AUnorderedMapConstIterator<K, T>(m_bucketArr, bucketIndex, m_bucketArr[bucketIndex].begin(), m_bucketCount);
		}

		AUnorderedMap<K, T>::AConstIterator end() const
		{
			size_t bucketIndex;
			for (size_t i = m_bucketCount - 1; i >= 0; i--)
			{
				if (m_bucketArr[i].GetCount() != 0)
				{
					bucketIndex = i;
					break;
				}
			}
			return AUnorderedMapConstIterator<K, T>(m_bucketArr, bucketIndex, m_bucketArr[bucketIndex].end(), m_bucketCount);
		}

		AUnorderedMap<K, T>::AConstIterator rbegin() const
		{
			size_t bucketIndex;
			for (size_t i = m_bucketCount - 1; i >= 0; i--)
			{
				if (m_bucketArr[i].GetCount() != 0)
				{
					bucketIndex = i;
					break;
				}
			}
			return AUnorderedMapConstIterator<K, T>(m_bucketArr, bucketIndex, m_bucketArr[bucketIndex].rbegin(), m_bucketCount);
		}

		AUnorderedMap<K, T>::AConstIterator rend() const
		{
			size_t bucketIndex;
			for (size_t i = 0; i < m_bucketCount; i++)
			{
				if (m_bucketArr[i].GetCount() != 0)
				{
					bucketIndex = i;
					break;
				}
			}
			return AUnorderedMapConstIterator<K, T>(m_bucketArr, bucketIndex, m_bucketArr[bucketIndex].rend(), m_bucketCount);
		}


		AUnorderedMap<K, T>& operator=(const AUnorderedMap<K, T>& other)
		{
			if (other == *this)
			{
				return *this;
			}

			delete[] m_bucketArr;
			m_bucketArr = other.m_bucketArr;
			other.m_equalsFunc = other.m_equalsFunc;
			m_hash = other.m_hash;
			return *this;
		}

		AUnorderedMap<K, T>& operator=(AUnorderedMap<K, T>&& other)
		{
			AE_PROFILE_FUNCTION();
			if(other == *this)
			{
				return *this;
			}

			delete[] m_bucketArr;
			m_bucketArr = other.m_bucketArr;

			other.m_bucketArr = nullptr;
			other.m_bucketCount = 0;
			other.m_count = 0;
			return *this;
		}
		
		bool operator==(const AUnorderedMap<K, T>& other) const
		{
			for (size_t i = 0; i < m_bucketCount; i++)
			{
				if (m_bucketArr[i] != other.m_bucketArr[i])
				{
					return false;
				}
			}

			return true;
		}

		bool operator!=(const AUnorderedMap<K, T>& other) const
		{
			return !(operator==(other));
		}

	private:

		static bool DefaultEquals(const K& e1, const K& e2)
		{
			return e1 == e2;
		}

		static int DefaultCompress(long hash, size_t size)
		{
			return (7l * hash + 31l) % size;
		}

		int GetBucketIndex(const K& key) const
		{
			AE_PROFILE_FUNCTION();
			return m_compressFunc((int)m_hash(key), m_bucketCount);
		}

		void AddAKeyElementPair(AKeyElementPair<K, T> pair, int index)
		{
			m_bucketArr[index].Add(pair);
		}

		//returns true if rehashed hash map
		bool CheckLoadFactor()
		{
			size_t loadFactor = m_count / m_bucketCount;

			if (loadFactor >= UNORDERED_MAP_MAX_LOAD_FACTOR)
			{
				Rehash();
				return true;
			}

			return false;
		}

		void Rehash()
		{
			size_t numBuckets = (size_t)((float)m_bucketCount * 1.5f);

			if (numBuckets == m_bucketCount)
			{
				numBuckets += 5;
			}

			size_t oldBucketCount = m_bucketCount;
			ASinglyLinkedList<AKeyElementPair<K, T>>* oldBuckets = m_bucketArr;
			m_bucketArr = new ASinglyLinkedList<AKeyElementPair<K, T>>[numBuckets];
			m_count = 0;
			m_bucketCount = numBuckets;

			for (size_t i = 0; i < oldBucketCount; i++)
			{
				for (AKeyElementPair<K, T>& pair : oldBuckets[i])
				{
					Add(pair.GetKey(), pair.GetElement());
				}
			}

			delete[] oldBuckets;
		}


		ADelegate<bool(const K&, const K&)> m_equalsFunc;
		ADelegate<int(long, size_t)> m_compressFunc;
		std::hash<K> m_hash;
		ASinglyLinkedList<AKeyElementPair<K, T>>* m_bucketArr;
		size_t m_bucketCount;
		size_t m_count;
	};
	
}