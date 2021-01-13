#pragma once
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Data Struct/ADoublyLinkedList.h"
#include "AstralEngine/Data Struct/AUniqueRef.h"
#include "AstralEngine/Data Struct/ASparseSet.h"
#include "AstralEngine/Data Struct/AKeyElementPair.h"


namespace AstralEngine
{
	class CallbackComponent;
	class NativeScript;

	template<typename...>
	class Storage;

	template<typename Entity, typename Component>
	class Storage<Entity, Component> : public ASparseSet<Entity>
	{
		template<bool Const>
		class StorageIterator;
	
	public:
		using AIterator = StorageIterator<false>;
		using AConstIterator = StorageIterator<true>;
	
		Storage() : ASparseSet<Entity>(ADelegate<size_t(const Entity)>(&ToIntegral)) { }
	
		virtual ~Storage() { }
	
		template<typename... Args>
		Component& Emplace(const Entity& e, Args&&... args)
		{
			AE_PROFILE_FUNCTION();

			Component* comp;

			if (Contains(e))
			{
				comp = &m_components[GetIndex(e)].Emplace(std::forward<Args>(args)...);
			}
			else
			{
				ASparseSet<Entity>::Add(e);

				m_components.Add(ADoublyLinkedList<Component>());
				comp = &m_components[GetIndex(e)].Emplace(std::forward<Args>(args)...);
			}

			if constexpr (std::is_base_of_v<CallbackComponent, Component>)
			{
				comp->OnCreate();
			}

			return *comp;
		}
	
		const ADoublyLinkedList<Component>& Remove(const Entity& e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Storage does not contain provided Entity");
	
			//take last component and move it to the index to remove then remove the last element to be more efficient
			auto last = std::move(m_components[m_components.GetCount() - 1]);
			m_components[GetIndex(e)] = std::move(last);
			ADoublyLinkedList<Component>& list = m_components[m_components.GetCount() - 1];
			m_components.Remove(m_components.GetCount() - 1);
			ASparseSet<Entity>::Remove(e);

			if constexpr (std::is_base_of_v<CallbackComponent, Component>) 
			{
				for (Component& c : list)
				{
					c.OnDestroy();
				}
			}

			return list;
		}
	
		//remove first component from the list
		Component& RemoveComponent(const Entity& e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Storage does not contain provided Entity");
	
			ADoublyLinkedList<Component>& list = m_components[GetIndex(e)];
			Component& comp = list[0];
			list.Remove(list.begin());
	
			if (list.IsEmpty())
			{
				Remove(e);
			}

			if constexpr (std::is_base_of_v<CallbackComponent, Component>)
			{
				comp.OnDestroy();
			}

			return comp;
		}
	
		//remove specified component
		void RemoveComponent(const Entity& e, const Component& comp)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Storage does not contain provided Entity");

			ADoublyLinkedList<AKeyElementPair<unsigned int, Component>>& list = m_components[GetIndex(e)];
			AE_CORE_ASSERT(list.Contains(comp), "Trying to removie a component from an entity which does not own this component");
			
			for (auto it = list.begin(); it != list.end(); it++)
			{
				if (*it == comp)
				{
					list.Remove(it);
					break;
				}
			}

			if (list.IsEmpty())
			{
				Remove(e);
			}

			if constexpr (std::is_base_of_v<CallbackComponent, Component>)
			{
				comp.OnDestroy();
			}

		}
	
		//provides the first component found for the type
		Component& Get(const Entity& e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Storage does not contain provided Entity");
			AE_CORE_ASSERT(!m_components.IsEmpty(), "No component of the provided type is linked to the entity provided");
			return m_components[GetIndex(e)][0];
		}
	
		ADoublyLinkedList<Component>& GetList(const Entity& e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Storage does not contain provided Entity");
			return m_components[GetIndex(e)];
		}

		void Clear()
		{
			ASparseSet<Entity>::Clear();
			m_components.Clear();
		}
	
		AIterator begin()
		{
			auto arrIt = m_components.begin();

			while ((*arrIt).IsEmpty())
			{
				++arrIt;
			}

			return AIterator(arrIt, (*arrIt).begin());
		}
	
		AIterator end()
		{
			auto arrIt = m_components.end();
			auto linkedIt = (*(*arrIt).rbegin()).end();
			return AIterator(arrIt, linkedIt);
		}
	
		AConstIterator begin() const
		{
			auto arrIt = m_components.begin();

			while ((*arrIt).IsEmpty())
			{
				++arrIt;
			}

			return AConstIterator(arrIt, (*arrIt).begin());
		}
	
		AConstIterator end() const
		{
			auto arrIt = m_components.end();
			auto linkedIt = (*(*arrIt).rbegin()).end();
			return AConstIterator(arrIt, linkedIt);
		}
	
		void Swap(const Entity lhs, const Entity rhs)
		{
			AE_PROFILE_FUNCTION();
			ADoublyLinkedList<Component> temp = std::move(m_components[GetIndex(lhs)]);
			m_components[GetIndex(lhs)] = std::move(m_components[GetIndex(rhs)]);
			m_components[GetIndex(rhs)] = std::move(temp);
			ASparseSet<Entity>::Swap(lhs, rhs);
		}
	
	
	private:
	
		template<bool Const>
		class StorageIterator final
		{
			friend class Storage<Entity, Component>;
	
			using Ref = std::conditional_t<Const, const Component, Component>;
	
			using ArrIteratorType = std::conditional_t<Const,
				typename ADynArr<ADoublyLinkedList<Component>>::AConstIterator,
				typename ADynArr<ADoublyLinkedList<Component>>::AIterator>;
	
			using LinkedIteratorType = std::conditional_t<Const,
				typename ADoublyLinkedList<Component>::AConstIterator,
				typename ADoublyLinkedList<Component>::AIterator>;
	
			StorageIterator(ArrIteratorType& arrIt,
				LinkedIteratorType& linkedIt)
				: m_arrIt(arrIt), m_linkedIt(linkedIt) { }
	
		public:
	
			StorageIterator() = default;
	
			StorageIterator& operator++()
			{
				++m_linkedIt;
				if (m_linkedIt == (*m_arrIt).end())
				{
					++m_arrIt;
					m_linkedIt = (*m_arrIt).begin();
				}
	
				return *this;
			}
	
			StorageIterator operator++(int)
			{
				StorageIterator copy = *this;
				operator++();
				return copy;
			}
	
			StorageIterator& operator--()
			{
				--m_linkedIt;
				if (m_linkedIt == (*m_arrIt).rend())
				{
					--m_arrIt;
					m_linkedIt = (*m_arrIt).rbegin();
				}
				return *this;
			}
	
			StorageIterator operator--(int)
			{
				StorageIterator copy = *this;
				operator--();
				return copy;
			}
	
			StorageIterator& operator+=(size_t val)
			{
				for (size_t i = 0; i < val; i++)
				{
					this->operator++();
				}
				return *this;
			}
	
			StorageIterator operator+(size_t val)
			{
				StorageIterator copy = *this;
				return (copy += val);
			}
	
			StorageIterator& operator-=(size_t val)
			{
				for (size_t i = 0; i < val; i++)
				{
					this->operator--();
				}
				return *this;
			}
	
			StorageIterator operator-(size_t val)
			{
				StorageIterator copy = *this;
				return (copy -= val);
			}
	
			bool operator==(const StorageIterator& other) const
			{
				return m_arrIt == other.m_arrIt && m_linkedIt == other.m_linkedIt;
			}
	
			bool operator!=(const StorageIterator& other) const
			{
				return !(*this == other);
			}
	
			Ref operator*() const
			{
				return *m_linkedIt;
			}
	
		private:
			ArrIteratorType m_arrIt;
			LinkedIteratorType m_linkedIt;
		};
	
		ADynArr<ADoublyLinkedList<Component>> m_components;
	};
	
	template<typename Entity>
	class Storage<Entity, AReference<CallbackComponent>> : public ASparseSet<Entity>
	{
		template<bool Const>
		class StorageIterator;

	public:
		using AIterator = StorageIterator<false>;
		using AConstIterator = StorageIterator<true>;

		Storage() : ASparseSet<Entity>(ADelegate<size_t(const Entity)>(&ToIntegral)) { }

		virtual ~Storage() { }

		template<typename Component>
		AReference<CallbackComponent> Emplace(const Entity& e, AReference<Component>& ref)
		{
			AE_PROFILE_FUNCTION();

			if (Contains(e))
			{
				m_components[GetIndex(e)].AddLast({ IndexProvider<Component>::GetIndex() , ref });
			}
			else
			{
				m_components.Add(ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>());
				m_components[m_components.GetCount() - 1].AddLast({ IndexProvider<Component>::GetIndex() , ref });
				ASparseSet<Entity>::Add(e);
			}

			return (AReference<CallbackComponent>)ref;
		}

		const ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>& Remove(const Entity& e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Storage does not contain provided Entity");

			//take last component and move it to the index to remove then remove the last element to be more efficient
			auto last = std::move(m_components[m_components.GetCount() - 1]);
			m_components[GetIndex(e)] = std::move(last);
			ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>& list
				= m_components[m_components.GetCount() - 1];
			m_components.Remove(m_components.GetCount() - 1);
			ASparseSet<Entity>::Remove(e);

			for (auto& pair : list)
			{
				AReference<CallbackComponent>& comp = pair.GetElement();
				comp->OnDestroy();
			}

			return list;
		}

		//remove first component from the list
		template<typename Component>
		AReference<CallbackComponent> RemoveComponent(const Entity& e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Storage does not contain provided Entity");

			ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>& list = m_components[GetIndex(e)];
			AReference<CallbackComponent> comp;

			auto it = list.begin();
			for (; it != list.end(); it++)
			{
				if ((*it).GetKey() == IndexProvider<Component>::GetIndex())
				{
					comp = (*it).GetElement();
					break;
				}
			}

			AE_CORE_ASSERT(it != list.end(), "Cannot find component of type provided");

			list.Remove(it);

			if (list.IsEmpty())
			{
				Remove(e);
			}
			return comp;
		}

		//remove specified component
		template<typename Component>
		void RemoveComponent(const Entity& e, const Component& comp)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Storage does not contain provided Entity");

			ADoublyLinkedList<AKeyElementPair<unsigned int, CallbackComponent*>>& list = m_components[GetIndex(e)];
			AE_CORE_ASSERT(list.Contains((CallbackComponent*)&comp), "Trying to removie a component from an entity which does not own this component");

			for (auto it = list.begin(); it != list.end(); it++)
			{
				if ((*it).GetKey() == IndexProvider<Component>::GetIndex() 
					&& comp == *((AReference<CallbackComponent>)(*it).GetElement()).Get())
				{
					list.Remove(it);
					break;
				}
			}

			auto it = list.begin();
			for (; it != list.end(); it++)
			{
				if ((*it).GetKey() == IndexProvider<Component>::GetIndex()
					&& comp == *((AReference<CallbackComponent>)(*it).GetElement()).Get())
				{
					comp = (*it).GetElement();
					break;
				}
			}

			AE_CORE_ASSERT(it != list.end(), "Cannot find component provided");

			list.Remove(it);

			if (list.IsEmpty())
			{
				Remove(e);
			}

			comp.OnDestroy();
		}


		const ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>& Get(const Entity& e) const
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Storage does not contain provided Entity");
			return m_components[GetIndex(e)];
		}

		CallbackComponent& Get(const Entity& e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Storage does not contain provided Entity");
			return *m_components[GetIndex(e)][0].GetElement().Get();
		}

		ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>& GetList(const Entity& e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Storage does not contain provided Entity");
			return m_components[GetIndex(e)];
		}

		void Clear()
		{
			ASparseSet<Entity>::Clear();
			m_components.Clear();
		}

		AIterator begin()
		{
			auto arrIt = m_components.begin();

			while ((*arrIt).IsEmpty())
			{
				++arrIt;
			}

			return AIterator(arrIt, (*arrIt).begin());
		}

		AIterator end()
		{
			auto arrIt = m_components.end();
			auto linkedIt = (*(*arrIt).rbegin()).end();
			return AIterator(arrIt, linkedIt);
		}

		AConstIterator begin() const
		{
			auto arrIt = m_components.begin();

			while ((*arrIt).IsEmpty())
			{
				++arrIt;
			}

			return AConstIterator(arrIt, (*arrIt).begin());
		}

		AConstIterator end() const
		{
			auto arrIt = m_components.end();
			auto linkedIt = (*(*arrIt).rbegin()).end();
			return AConstIterator(arrIt, linkedIt);
		}

		void Swap(const Entity lhs, const Entity rhs)
		{
			AE_PROFILE_FUNCTION();
			auto temp = std::move(m_components[GetIndex(lhs)]);
			m_components[GetIndex(lhs)] = std::move(m_components[GetIndex(rhs)]);
			m_components[GetIndex(rhs)] = std::move(temp);
			SparseSet<Entity>::Swap(lhs, rhs);
		}


	private:

		template<bool Const>
		class StorageIterator final
		{
			friend class Storage<Entity, AReference<NativeScript>>;

			using Ref = std::conditional_t<Const, const AKeyElementPair<unsigned int, AReference<CallbackComponent>>,
				AKeyElementPair<unsigned int, AReference<CallbackComponent>>>;

			using ArrIteratorType = std::conditional_t<Const,
				ADynArr<ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>>::AConstIterator,
				ADynArr<ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>>::AIterator>;

			using LinkedIteratorType = std::conditional_t<Const,
				ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>::AConstIterator,
				ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>::AIterator>;

			StorageIterator(ArrIteratorType& arrIt,
				LinkedIteratorType& linkedIt)
				: m_arrIt(arrIt), m_linkedIt(linkedIt) { }

		public:

			StorageIterator() = default;

			StorageIterator& operator++()
			{
				++m_linkedIt;
				if (m_linkedIt == (*m_arrIt).end())
				{
					++m_arrIt;
					m_linkedIt = (*m_arrIt).begin();
				}

				return *this;
			}

			StorageIterator operator++(int)
			{
				StorageIterator copy = *this;
				operator++();
				return copy;
			}

			StorageIterator& operator--()
			{
				--m_linkedIt;
				if (m_linkedIt == (*m_arrIt).rend())
				{
					--m_arrIt;
					m_linkedIt = (*m_arrIt).rbegin();
				}
				return *this;
			}

			StorageIterator operator--(int)
			{
				StorageIterator copy = *this;
				operator--();
				return copy;
			}

			StorageIterator& operator+=(size_t val)
			{
				for (size_t i = 0; i < val; i++)
				{
					this->operator++();
				}
				return *this;
			}

			StorageIterator operator+(size_t val)
			{
				StorageIterator copy = *this;
				return (copy += val);
			}

			StorageIterator& operator-=(size_t val)
			{
				for (size_t i = 0; i < val; i++)
				{
					this->operator--();
				}
				return *this;
			}

			StorageIterator operator-(size_t val)
			{
				StorageIterator copy = *this;
				return (copy -= val);
			}

			bool operator==(const StorageIterator& other) const
			{
				return m_arrIt == other.m_arrIt && m_linkedIt == other.m_linkedIt;
			}

			bool operator!=(const StorageIterator& other) const
			{
				return !(*this == other);
			}

			Ref operator*() const
			{
				return *m_linkedIt;
			}

		private:
			ArrIteratorType m_arrIt;
			LinkedIteratorType m_linkedIt;
		};

		ADynArr<ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>> m_components;
	};

}