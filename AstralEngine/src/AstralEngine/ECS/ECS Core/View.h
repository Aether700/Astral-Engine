#pragma once
#include "Storage.h"
#include "ECSUtils.h"

#include <algorithm>
#include <type_traits>

namespace AstralEngine
{
	template<typename Entity>
	class Registry;

	/*provide a general view declaration
	  other classes will provide specializations of this class
	*/
	template<typename ...>
	class View;


	/*View with Excluded type list and multiple Components

	  the view will only return entities that have all of the components listed and none of the excluded ones
	*/
	template<typename Entity, typename... Exclude, typename... Component>
	class View<Entity, TypeList<Exclude...>, Component...>
	{
		friend class Registry<Entity>;

		/*given the component type provided, std::conditional_t will return the type
		  const Storage<Entity, std::remove_const_t<Comp>> if Comp is constant or Storage<Entity, Comp>> 
		  if the type is not constant
		*/
		template<typename Comp>
		using PoolType = 
			std::conditional_t<std::is_const_v<Comp>, const Storage<Entity, std::remove_const_t<Comp>>,
			Storage<Entity, Comp>>;
			
		template<typename Comp>
		using ComponentIterator = decltype(std::declval<PoolType<Comp>>().begin());
		using UnderlyingIterator = typename ASparseSet<Entity>::AIterator;

		//"sizeof...(Component)" returns the number of components in the pack "Component"
		using UncheckedType = std::array<const ASparseSet<Entity>*, (sizeof...(Component) - 1)>;
		using FilterType = std::array<const ASparseSet<Entity>*, sizeof...(Exclude)>;

		class ViewIterator;

	public:
		using AIterator = ViewIterator;

		//returns the number of entities with the provided component
		template<typename Comp>
		size_t GetCount() const
		{
			return std::get<PoolType<Comp>*>->GetCount();
		}

		//provides an estimate of how many entities are in the view
		size_t GetCount() const
		{
			return std::min<auto>({ std::get<PoolType<Component>*>(m_pools)->GetCount()... });
		}

		/* tries to checks if the view is empty
		
		   this function is not 100% accurate thus, the view might be empty 
		   and not return entities even if this function returns false
		*/
		template<typename... Comp>
		bool IsEmpty() const
		{
			if constexpr(sizeof...(Comp) == 0)
			{
				return (std::get<PoolType<Comp>*>(m_pools)->IsEmpty() || ...);
			}
			else
			{
				return (std::get<PoolType<Comp>*>(m_pools)->IsEmpty() && ...);
			}
		}

		AIterator begin() const
		{
			const auto& view = GetCanditate();
			const FilterType ignore = { std::get<PoolType<Exclude>*>(m_pools)... };
			return AIterator(view, Unchecked(view), ignore, view.begin());
		}


		AIterator end() const
		{
			const auto& view = GetCanditate();
			const FilterType ignore = { std::get<PoolType<Exclude>*>(m_pools)... };
			return AIterator(view, Unchecked(view), ignore, view.end());
		}

		bool Contains(const Entity e) const
		{
			return (std::get<PoolType<Component>*>(m_pools)->Contains(e) && ...)
				&& (!std::get<PoolType<Exclude>*>(m_pools)->Contains(e) && ...);
		}

		template<typename... Comp>
		decltype(auto) Get(const Entity e) const
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(Contains(e), "Entity provided not contained within View");

			//if no Comp type
			if constexpr(sizeof... (Comp) == 0)
			{
				//assert there is only one Component type and return a "list" of the Components 
				//(should be singular at this point) that is linked to the entity provided
				static_assert(sizeof... (Component) == 1);
				return (std::get<PoolType<Component>*>(m_pools)->Get(e), ...);
			}
			else if constexpr(sizeof...(Comp) == 1)
			{
				//if one Comp type return a list of all the components linked to the entity provided
				return (std::get<PoolType<Comp>*>(m_pools)->Get(e), ...);
			}
			else 
			{
				/*keep else to allow compiler optimization from the if-constexpr

				  return a tuple of all types returned by this function and fill it by recursively 
				  calling Get<Comp>(e) for every Comp type
				*/
				return std::tuple<decltype(Get<Comp>({}))...> {Get<Comp>(e)...};
			}
		}

		//Func must be a function with the signature void(Type&...) or void (Entity, Type&...)
		template<typename Func>
		void ForEach(Func function)
		{
			const auto& view = GetCanditate();
			//go through each components and call the other ForEach function below on it
			((std::get<PoolType<Component>*>(m_pools) == &view ? ForEach<Component>(std::move(function)) : void()), ...);
		}

		/*calls the function provided on every Entity which contains the provided type

		  the function provided must have one of the two signatures below
		  void (Type&...)
		  void (Entity, Type&...)
		*/
		template<typename Comp, typename Func>
		void ForEach(Func function)
		{
			//Get a type list which is not empty
			using NonEmptyType = typename TypeListCat<std::conditional_t<std::is_empty_v<Component>, TypeList<>, TypeList<Component>>...>::Type;
			Traverse<Comp>(std::move(function), NonEmptyType{});
		}

	private:

		class ViewIterator
		{
			friend class View<Entity, TypeList<Exclude...>, Component...>;

		public:
			using Ptr = const Entity*;
			using Ref = const Entity&;

			ViewIterator() { }

			ViewIterator& operator++()
			{
				while (++m_it != m_view->end() && !IsValid())
				{
					/*while the current entity is not valid and that
					  we have not reached the end of the view increment the iterator
					*/
				}
				return *this;
			}

			ViewIterator operator++(int)
			{
				ViewIterator copy = *this;
				this->operator++();
				return copy;
			}

			ViewIterator& operator--()
			{
				while (--m_it != m_view->begin() && !IsValid())
				{
					/*while the current entity is not valid and that
					  we have not reached the start of the view decrement the iterator
					*/
				}
				return *this;
			}

			ViewIterator operator--(int)
			{
				ViewIterator copy = *this;
				this->operator--();
				return copy;
			}

			bool operator==(const ViewIterator& other) const
			{
				return m_it == other.m_it;
			}

			bool operator!=(const ViewIterator& other) const
			{
				return !(*this == other);
			}

			Ref operator*() const
			{
				return m_it.operator*();
			}

			Ptr operator->() const
			{
				return m_it.operator->();
			}

		private:
			ViewIterator(const ASparseSet<Entity>& candidate, UncheckedType other, FilterType ignore, UnderlyingIterator it)
				: m_view(&candidate), m_unchecked(other), m_filter(ignore), m_it(it)
			{
				//if starting iterator is not valid and we are not at 
				//the end of the current view increment the iterator
				//(will increment until we find a valid Entity
				if (m_it != m_view->end() && !IsValid())
				{
					this->operator++();
				}
			}

			/* returns true if all of the uncheck array contains the following entity
			   and if none of the filter array contains the entity, false otherwise
			*/
			bool IsValid() const
			{
				return std::all_of(m_unchecked.cbegin(), m_unchecked.cend(),
					[entity = *m_it](const ASparseSet<Entity>* curr){ return curr->Contains(entity); })
					&& std::none_of(m_filter.cbegin(), m_filter.cend(),
						[entity = *m_it](const ASparseSet<Entity>* curr){ return curr->Contains(entity); });
			}

			const ASparseSet<Entity>* m_view;
			UncheckedType m_unchecked;
			FilterType m_filter;
			UnderlyingIterator m_it;
		};

		View(Storage<Entity, std::remove_const_t<Component>>&... component,
			Storage<Entity, std::remove_const_t<Exclude>>&... excludedPool) : m_pools{ &component..., &excludedPool... } { }

		/* returns the Storage with the least entities (casted as a sparse set) since the view only deals with entities 
		   that contain all components thus, having a smaller set of entities boosts performance
		*/
		const ASparseSet<Entity>& GetCanditate() const
		{
			AE_PROFILE_FUNCTION();
			//returns the Storage containing the least entities casted as a SparseSet
			return *(std::min)({ static_cast<const ASparseSet<Entity>*>(std::get<PoolType<Component>*>(m_pools))... },
				[](const auto* lhs, const auto* rhs)
				{
					return lhs->GetCount() < rhs->GetCount();
				});
		}

		UncheckedType Unchecked(const ASparseSet<Entity>& view) const
		{
			size_t pos = 0;
			UncheckedType other;
			/*fill the array of the UncheckedType with all the PoolTypes of the 
			  Components not refering to the view provided and return the UncheckedType array
			*/
			((std::get<PoolType<Component>*>(m_pools) == &view ? nullptr
				: (other[pos++] = std::get<PoolType<Component>*>(m_pools))), ...);
			return other;
		}

		/*decltype(auto) forwards the complete type to be returned

		  ex: if returned value is "int const&",
		  auto would simply return "int" but decltype(auto) returns "int const&"

		  Note that because of the if-constexpr in the function not all arguments of the functions will be used
		*/
		template<typename Comp, typename Other>
		decltype(auto) Get(ComponentIterator<Comp> it, PoolType<Other>* cPool, const Entity e) const
		{
			AE_PROFILE_FUNCTION();
			/* if the types are the same return "*it" otherwise return "cPool->get(e)"

			  the constexpr is there to allow the compiler to optimize even further the code.
			  if the if branch is true we can discard the else at compile time while if it's
			  false only use the else and disregard the other statement
			*/
			if constexpr (std::is_same_v<Comp, Other>)
			{
				return *it;
			}
			else
			{
				//here because of the constexpr, use else so that the compiler only picks one or the other of the statements
				return cPool->get(e);
			}
		}

		//uses the same if-constexpr as the get function above
		template<typename Comp, typename Func, typename... Type>
		void Traverse(Func function, TypeList<Type...>) const
		{
			AE_PROFILE_FUNCTION();
			/* given a list of types B1 to BN, if sizeof(B) == 0 will return std::false_type
			   otherwise, will return the first Bi in B1 to BN such that bool(Bi::value) == true or,
			   if no such Bi was found, will return BN

			   for disjunction_v will return Bi::value in the case that Bi would be returned

			   in otherwords, if we have the Comp type in our list of Types then the if is true,
			   otherwise it's false
			*/
			if constexpr (std::disjunction_v<std::is_same<Comp, Type>...>)
			{
				auto it = std::get<PoolType<Comp>*>(m_pools)->begin();

				for (const auto entity : static_cast<const ASparseSet<Entity>&>(*std::get<PoolType<Comp>*>(m_pools)))
				{
					auto curr = it++;

					//return true if every Component pool contains the current entity AND if every exclude pool does not
					if (((std::is_same_v<Comp, Component> || std::get<PoolType<Component>*>(m_pools)->Contains(entity)) && ...)
						&& (!std::get<PoolType<Exclude>*>(m_pools)->Contains(entity) && ...))
					{
						//if Func can be invoked with the provided types as arguments (the decltype(Get<Type>())...)
						if constexpr (std::is_invocable_v<Func, decltype(Get<Type>())...>)
						{
							function(Get<Comp, Type>(curr, std::get<PoolType<Type>*>(m_pools), entity)...);
						}
						else
						{
							//else call it with arguments "entity, <list of arguments>"
							function(entity, Get<Comp, Type>(curr, std::get<PoolType<Type>*>(m_pools), entity)...);
						}
					}
				}
			}
			else
			{
				//do same loop as above but call the function with different arguments
				for (const auto entity : static_cast<const ASparseSet<Entity>&>(*std::get<PoolType<Comp>*>(m_pools)))
				{
					if (((std::is_same_v<Comp, Component> || std::get<PoolType<Component>*>(m_pools)->Contains(entity)) && ...)
						&& (!std::get<PoolType<Exclude>*>(m_pools)->Contains(entity) && ...))
					{
						if constexpr (std::is_invocable_v < Func, decltype(Get<Type>({}))... > )
						{
							function(std::get<PoolType<Type>*>(m_pools)->Get(entity)...);
						}
						else
						{
							function(entity, std::get<PoolType<Type>*>(m_pools)->Get(entity)...);
						}
					}
				}
			}
		}


		const std::tuple<PoolType<Component>*..., PoolType<Exclude>*...> m_pools;
	};

	/*View with no Excluded type and a single component type

	  the view will only return entities that have the component specified
	*/
	template<typename Entity, typename Component>
	class View<Entity, TypeList<>, Component>
	{
		friend class Registry<Entity>;

		using PoolType = std::conditional_t<std::is_const_v<Component>, const Storage<Entity, 
			std::remove_const_t<Component>>, Storage<Entity, Component>>;

	public:
		using AIterator = typename ASparseSet<Entity>::AIterator;

		size_t GetCount() const { return m_pool->GetCount(); }

		bool IsEmpty() const { return m_pool->IsEmpty(); }

		AIterator begin() const { return m_pool->ASparseSet<Entity>::begin(); }

		AIterator end() const { return m_pool->ASparseSet<Entity>::end(); }

		Entity operator[](size_t index) const
		{
			return *(begin() + index);
		}

		bool Contains(const Entity e) const
		{
			return m_pool->Contains(e);
		}

		template<typename Comp = Component>
		decltype(auto) Get(const Entity e) const
		{
			AE_PROFILE_FUNCTION();
			static_assert(std::is_same_v<Comp, Component>);
			AE_CORE_ASSERT(Contains(e), "Entity provided not contained in View");
			return m_pool->Get(e);
		}

		template<typename Func>
		void ForEach(Func function) const
		{
			if constexpr (std::is_empty_v<Component>)
			{
				if constexpr (std::is_invocable_v<Func>)
				{
					for (size_t i = 0; i < m_pool->GetCount(); i++)
					{
						function();
					}
				}
				else
				{
					for (Entity e : *this)
					{
						function(e);
					}
				}
			}
			else
			{
				if constexpr(std::is_invocable_v<Func, decltype(Get({}))>)
				{
					for (auto&& component : *m_pool)
					{
						function(component);
					}
				}
				else
				{
					//iterator over the components
					auto it = m_pool->begin();

					//call function with entity and component as arguments
					for (const auto entity : *this)
					{
						function(entity, *(it++));
					}

				}
			}
		}

	private:
		View(PoolType& ref) : m_pool(&ref) { }

		PoolType* m_pool;
	};

}