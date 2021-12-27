#pragma once
#include "ADelegate.h"

namespace AstralEngine
{
	template<typename K, typename T>
	struct AKeyElementPair
	{		
		AKeyElementPair() : m_equalsFunc(&DefaultEquals) { }

		AKeyElementPair(const AKeyElementPair<K, T>& other) : m_key(other.m_key),
			m_element(other.m_element), m_equalsFunc(other.m_equalsFunc) { }

		AKeyElementPair(const K& k, ADelegate<bool(const K&, const K&)> equals = DefaultEquals)
			: m_equalsFunc(equals), m_key(k) { }

		AKeyElementPair(const K& k, const T& e, ADelegate<bool(const K&, const K&)> equals = DefaultEquals)
			: m_equalsFunc(equals), m_key(k), m_element(e) { }
		
		template<typename... Args>
		AKeyElementPair(const K& k, Args... args, ADelegate<bool(const K&, const K&)> equals = DefaultEquals)
			: m_equalsFunc(equals), m_key(k), m_element(std::forward<Args>(args)...) { }

		AKeyElementPair(const K& k, std::function<bool(const K&, const K&)> equals)
			: m_equalsFunc(equals), m_key(k), m_element() { }

		AKeyElementPair(const K& k, const T& e, std::function<bool(const K&, const K&)> equals)
			: m_equalsFunc(equals), m_key(k), m_element(e) { }

		template<typename... Args>
		AKeyElementPair(const K& k, Args... args, std::function<bool(const K&, const K&)> equals)
			: m_equalsFunc(equals), m_key(k), m_element(std::forward<Args>(args)...) { }

		bool operator==(const AKeyElementPair<K, T>& other)
		{
			return m_equalsFunc(m_key, other.m_key);
		}

		bool operator!=(const AKeyElementPair<K, T>& other)
		{
			return !(*this == other);
		}


		K& GetKey() { return m_key; }
		T& GetElement() { return m_element; }
		const K& GetKey() const { return m_key; }
		const T& GetElement() const { return m_element; }

	private:
		static bool DefaultEquals(const K& k1, const K& k2) { return k1 == k2; }

		K m_key;
		T m_element;
		ADelegate<bool(const K&, const K&)> m_equalsFunc;
	};

}