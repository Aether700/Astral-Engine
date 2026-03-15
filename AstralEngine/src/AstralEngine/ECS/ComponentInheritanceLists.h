#pragma once
#include "Scene.h"
#include "AstralEngine/Data Struct/ASinglyLinkedList.h"

namespace AstralEngine
{
	class AEntity;

	class ComponentPairBase
	{
	public:
		virtual ~ComponentPairBase() {}

		virtual size_t GetTypeID() const = 0;
	};

	class ComponentPairInstanceBase : public ComponentPairBase
	{
	public:
		ComponentPairInstanceBase(BaseEntity entityID, Scene* s);
		AEntity GetAEntity();

	private:
		BaseEntity m_entityID;
		Scene* m_scene;
	};

	template<typename PairBase>
	class ComponentPairListBase
	{
	public:
		using AIterator = typename ASinglyLinkedList<PairBase*>::AIterator;
		using AConstIterator = typename ASinglyLinkedList<PairBase*>::AConstIterator;

		ComponentPairListBase() {}
		ComponentPairListBase(ComponentPairListBase&& other) noexcept : m_pairs(std::move(other.m_pairs)) {}

		ComponentPairListBase(const ComponentPairListBase&) {}

		virtual ~ComponentPairListBase()
		{
			Clear();
		}

		bool IsEmpty() const { return m_pairs.IsEmpty(); }

		void Clear()
		{
			for (PairBase* ptr : m_pairs)
			{
				delete ptr;
			}
			m_pairs.Clear();
		}

		AIterator begin() { return m_pairs.begin(); }
		AIterator end() { return m_pairs.end(); }

		AConstIterator begin() const { return m_pairs.begin(); }
		AConstIterator end() const { return m_pairs.end(); }


	protected:
		template<typename PairInstance>
		PairInstance* Add(BaseEntity e, Scene* s)
		{
			PairInstance* pair = new PairInstance(e, s);
			m_pairs.Add(pair);
			return pair;
		}

		/// <summary>
		/// removes the component from the list and either deletes it or returns it
		/// </summary>
		/// <typeparam name="Component">The type of the component to remove</typeparam>
		/// <param name="deletePair">indicates if the pair removed 
		/// should be deleted. If not it will be returned as a void*</param>
		/// <returns>nullptr if the pair was deleted, the pair itself if it was 
		/// not deleted but found, if not found returns nullptr</returns>
		template<typename Component>
		void* Remove(bool deletePair = true)
		{
			size_t id = TypeInfo<Component>::ID();
			for (auto it = m_pairs.begin(); it != m_pairs.end(); it++)
			{
				if ((*it)->GetTypeID() == id)
				{
					ComponentPairBase* removed = *it;
					m_pairs.Remove(it);
					if (deletePair)
					{
						delete removed;
						return nullptr;
					}
					else
					{
						return removed;
					}
				}
			}

			return nullptr;
		}

		ComponentPairListBase& AssignmentOperator(const ComponentPairListBase& other)
		{
			Clear();
			m_pairs = other.m_pairs;
			return *this;
		}

		ComponentPairListBase& MoveOperator(ComponentPairListBase&& other) noexcept
		{
			Clear();
			m_pairs = std::move(other.m_pairs);
			return *this;
		}

		bool EqualOperator(const ComponentPairListBase& other) const
		{
			return m_pairs == other.m_pairs;
		}

	private:
		ASinglyLinkedList<PairBase*> m_pairs;
	};
}