#pragma once
#include "ECSUtils.h"
#include "Storage.h"

namespace AstralEngine
{
	template<typename Entity>
	class Registry;

	//groups are faster than views when containing multiple components
	template<typename...>
	class Group;

	//https://entt.docsforge.com/master/entity-component-system/#views-and-groups

	//non-owning group, should avoid usage since it uses more memory (other groups do not)
	template<typename Entity, typename... Exclude, typename... Component>
	class Group<Entity, ExcludeList<Exclude...>, GetList<Component...>>
	{
		friend class Registry<Entity>;

		template<typename Component>
		using PoolType = std::conditional_t<std::is_const_v<Component>,
			const Storage<Entity, std::remove_const_t<Component>>, Storage<Entity, Component>>;

	public:
		using AIterator = typename ASparseSet<Entity>::AIterator;

		//return count for a single component
		template<typename Comp>
		size_t GetCount() const
		{
			return std::get<PoolType<Comp>*>(m_pools)->GetCount();
		}

		//return the number of entities contained in the group
		size_t GetCount() const { return m_handler->GetCount(); }

		template<typename... Comp>
		bool IsEmpty() const
		{
			if constexpr (sizeof... (Comp) == 0)
			{
				return m_handler->IsEmpty();
			}
			else
			{
				return (std::get<PoolType<Comp>*>(m_pools)->IsEmpty() && ...);
			}
		}

		AIterator begin() const { return m_handler->begin(); }

		AIterator end() const { return m_handler->end(); }

		bool Contains(const Entity e) const { return m_handler->Contains(e); }

		//returns the components assigned to the entity
		template<typename... Comp>
		decltype(auto) Get(const Entity e) const
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "");

			if constexpr (sizeof... (Comp) == 1)
			{
				//need to unwrap the Type "packet" even if just one
				return (std::get<PoolType<Comp>*>(m_pools)->Get(e), ...);
			}
			else
			{
				//recursive call
				return std::tuple<decltype(Get<Comp>({}))...> { Get<Comp>(e)... };
			}
		}

		template<typename Func>
		void ForEach(Func function)
		{
			using ComponentTypeList = TypeListCat<std::conditional<std::is_empty_v<Component>, TypeList<>, TypeList<Component>>...>;
			Traverse(std::move(function), ComponentTypeList{});
		}

	private:
		Group(ASparseSet<Entity>& ref, Storage<Entity, std::remove_const_t<Component>>&... gpool)
			: m_handler(&ref), m_pools(gpool...) { }

		template<typename Func, typename... Weak>
		void Traverse(Func function, TypeList<Weak...>) const
		{
			AE_PROFILE_FUNCTION();
			for (const Entity e : *m_handler)
			{
				if constexpr (std::is_invocable_v < Func, decltype(Get<Weak>({}))... > )
				{
					function(std::get<PoolType<Weak>*>(m_pools)->Get(e)...);
				}
				else
				{
					function(e, std::get<PoolType<Weak>*>(m_pools)->Get(e)...);
				}
			}
		}

		ASparseSet<Entity>* m_handler;
		const std::tuple<PoolType<Component>*...> m_pools;
	};

	/*owning group (not as in owning the memory), 
	  will move around and manage the components it contains to optimize iteration

	  Owned are the components owned by the group while Component are the components observed by the group
	*/
	template<typename Entity, typename... Exclude, typename... Component, typename... Owned>
	class Group<Entity, ExcludeList<Exclude...>, GetList<Component...>, Owned...>
	{
		friend class Registry<Entity>;

		template<typename Comp>
		using PoolType = std::conditional_t<std::is_const_v<Comp>, 
			const Storage<Entity, std::remove_const_t<Comp>>, Storage<Entity, Comp>>;

		template<typename Comp>
		using ComponentIterator = decltype(std::declval<PoolType<Comp>>().begin());

	public:

		using AIterator = typename ASparseSet<Entity>::AIterator;

		//return count of a single component
		template<typename Comp>
		size_t GetCount() const
		{
			return std::get<PoolType<Comp>*>(m_pools)->GetCount();
		}

		//returns the number of entities that have the given components
		size_t GetCount() const
		{
			return *m_length;
		}

		template<typename... Comp>
		bool IsEmpty() const
		{
			if constexpr(sizeof...(Comp) == 0)
			{
				return *m_length == 0;
			}
			else
			{
				return (std::get<PoolType<Comp>*>(m_pools)->IsEmpty() && ...);
			}
		}

		AIterator begin() const
		{
			return std::get<0>(m_pools)->ASparseSet<Entity>::begin();
		}

		AIterator end() const
		{
			return std::get<0>(m_pools)->ASparseSet<Entity>::begin() += *m_length;
		}

		bool Contains(const Entity e) const
		{
			AE_PROFILE_FUNCTION();
			return std::get<0>(m_pools)->Contains(e) && (std::get<0>(m_pools)->GetIndex(e) < (*m_length));
		}

		template<typename... Comp>
		decltype(auto) Get(const Entity e) const
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Entity provided not contained within Group");

			if constexpr(sizeof...(Comp) == 1)
			{
				return (std::get<PoolType<Comp>*>(m_pools)->Get(e), ...);
			}
			else
			{
				//recursive call
				return std::tuple<decltype(Get<Comp>({}))...> { Get<Comp>(e)... };
			}
		}

		template<typename Func>
		void ForEach(Func function) const
		{
			AE_PROFILE_FUNCTION();
			using OwnedTypeList = TypeListCat<std::conditional_t<std::is_empty_v<Owned>, TypeList<>, TypeList<Owned>>...>;
			using ComponentTypeList = TypeListCat<std::conditional_t<std::is_empty_v<Component>,
				TypeList<>, TypeList<Component>>...>;

			Traverse(std::move(function), OwnedTypeList{}, ComponentTypeList{});
		}

	private:
		Group(const size_t& extend, Storage<Entity, std::remove_const_t<Owned>>&... ownedPool,
			Storage<Entity, std::remove_const_t<Component>>&... componentPool)
			: m_pools{ &ownedPool..., &componentPool... }, m_length(&extend) {	}

		template<typename Func, typename... Strong, typename... Weak>
		void Traverse(Func function, TypeList<Strong...>, TypeList<Weak...>) const
		{
			//it and data might not be used
			auto it = std::make_tuple((std::get<PoolType<Strong>*>(m_pools)->end() - *m_length)...);
			auto data = std::get<0>(m_pools)->SparseSet<Entity>::end() - *m_length;

			for (size_t i = 0; i < *m_length; i++)
			{
				if constexpr (std::is_invocable_v < Func, decltype(Get<Strong>({}))..., decltype(Get<Weak>({}))... > )
				{
					if constexpr (sizeof...(Weak) == 0)
					{
						func(*(std::get<ComponentIterator<Strong>>(it)++)...);
					}
					else
					{
						const auto entity = *(data++);
						function(*(std::get<ComponentIterator<Strong>>(it)++)...,
							std::get<PoolType<Weak>*>(m_pools)->Get(entity)...);
					}
				}
				else
				{
					const auto entity = *(data++);
					function(entity, *(std::get<ComponentIterator<Strong>>(it)++)...,
						std::get<PoolType<Weak>*>(m_pools)->Get(entity)...);
				}
			}
		}

		const std::tuple<PoolType<Owned>*..., PoolType<Component>*...> m_pools;
		const size_t* m_length;
		const size_t* m_super;
	};



}