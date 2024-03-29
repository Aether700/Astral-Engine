#pragma once
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Data Struct/AUniqueRef.h"
#include "AstralEngine/Data Struct/ASparseSet.h"
#include "AstralEngine/Data Struct/AKeyElementPair.h"

namespace AstralEngine
{
	template<typename...>
	class Storage;

	template<typename Entity, typename Component>
	class Storage<Entity, Component> : public ASparseSet<Entity>
	{
		template<bool Const>
		class StorageIterator;
	
	public:
		using AIterator = typename ResizableArr<Component>::AIterator;
		using AConstIterator = typename ResizableArr<Component>::AConstIterator;
	
		Storage() : ASparseSet<Entity>(ADelegate<size_t(const Entity)>(&ToIntegral)) { }
	
		virtual ~Storage() { }
	
		template<typename... Args>
		Component& Emplace(const Entity& e, Args&&... args)
		{
			AE_ECS_ASSERT(!ASparseSet<Entity>::Contains(e), "Entity already contains the provided component type");

			ASparseSet<Entity>::Add(e);
			m_components[ASparseSet<Entity>::GetIndex(e)] = Component(std::forward<Args>(args)...);

			return m_components[ASparseSet<Entity>::GetIndex(e)];
		}
	
		void Remove(const Entity& e)
		{
			AE_ECS_ASSERT(ASparseSet<Entity>::Contains(e), "Storage does not contain provided Entity");
			//take last component and move it to the index to remove then remove the last element to be more efficient
			Component last = std::move(m_components[m_components.GetCount() - 1]);
			m_components[ASparseSet<Entity>::GetIndex(e)] = std::move(last);
			m_components.RemoveAt(m_components.GetCount() - 1);
			ASparseSet<Entity>::Remove(e);
		}
	
		Component& Get(const Entity& e)
		{
			AE_ECS_ASSERT(ASparseSet<Entity>::Contains(e), "Storage does not contain provided Entity");
			return m_components[ASparseSet<Entity>::GetIndex(e)];
		}
	
		void Clear()
		{
			ASparseSet<Entity>::Clear();
			m_components.Clear();
		}
	
		AIterator begin()
		{
			return m_components.begin();
		}
	
		AIterator end()
		{
			return m_components.end();
		}
	
		AConstIterator begin() const
		{
			return m_components.begin();
		}
	
		AConstIterator end() const
		{
			return m_components.end();
		}
	
		void Swap(const Entity lhs, const Entity rhs)
		{
			
			Component temp = std::move(m_components[GetIndex(lhs)]);
			m_components[GetIndex(lhs)] = std::move(m_components[GetIndex(rhs)]);
			m_components[GetIndex(rhs)] = std::move(temp);
			ASparseSet<Entity>::Swap(lhs, rhs);
		}
	
	private:
		ResizableArr<Component> m_components;
	};
}