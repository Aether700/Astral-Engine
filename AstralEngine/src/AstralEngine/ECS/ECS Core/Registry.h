#pragma once
#include "AstralEngine/Data Struct/ADelegate.h"
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Data Struct/AUniqueRef.h"
#include "Storage.h"
#include "ECSUtils.h"
#include "View.h"
#include "Group.h"

#include <type_traits>
#include <iterator>
#include <algorithm>

namespace AstralEngine
{
	/*OnCreate, OnUpdate & OnDestroy returns a Sink where the delegates will be called on particular events
	  OnCreate & OnDestroy are managed by the registry itself (call OnCreate when adding components and OnDestroy when removing them)

	  add an "Update" function to call the OnUpdate function from the SignalHandler
	*/
	template<typename E>
	class Registry
	{
		using Entity = E;
	public:
		Entity CreateEntity()
		{
			AE_PROFILE_FUNCTION();
			if (m_destroyedPos.IsEmpty())
			{
				Entity e = (Entity) m_entities.GetCount();
				m_entities.Add(e);
				return m_entities[m_entities.GetCount() - 1];
			}

			Entity e = { m_destroyedPos[0] };
			m_entities[ToIntegral(m_destroyedPos[0])] = e;
			m_destroyedPos.RemoveAt(0);
			return e;
		}

		void DeleteEntity(const Entity e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(IsValid(e), "Invalid entity provided");
			RemoveAllComponents(e);

			//store old entity to recycle it's index and set that entity in the ADynArr to Null
			m_destroyedPos.AddFirst(e);
			m_entities[ToIntegral(e)] = Null;
		}

		bool IsValid(const Entity& e) const
		{
			AE_PROFILE_FUNCTION();
			size_t index = ToIntegral(e);
			return index < m_entities.GetCount() && m_entities[index] == e;
		}

		template<typename Component, typename ...Args>
		Component& EmplaceComponent(const Entity& e, Args&&... args)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(IsValid(e), "Invalid Entity provided to Registry");
			Component& comp = Assure<Component>().Emplace(*this, e, std::forward<Args>(args)...);
			return comp;
		}
		
		template<typename Component>
		void RemoveComponent(const Entity& e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(IsValid(e), "Invalid Entity provided to Registry"); 

			if constexpr (std::is_base_of_v<CallbackComponent, Component>)
			{
				Assure<AReference<CallbackComponent>>().RemoveComponent<Component>(*this, e);
			}
			else 
			{
				Assure<Component>().RemoveComponent<Component>(*this, e);
			}
		}

		template<typename Component>
		void RemoveComponent(const Entity& e, const Component& comp)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(IsValid(e), "Invalid Entity provided to Registry");
			Assure<Component>().RemoveComponent(e, comp);
		}

		template<typename... Component>
		bool HasComponent(const Entity& e) const
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(IsValid(e), "Invalid entity provided to registry");
			return (Assure<Component>().Contains(e) && ...);
		}

		template<typename... Component>
		decltype(auto) GetComponent(const Entity& e)
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(IsValid(e), "Invalid Entity provided to Registry"); 
			
			if constexpr (sizeof...(Component) == 1)
			{
				return (Assure<Component>().Get(e), ...);
			}
			else
			{
				return std::forward_as_tuple(GetComponent<Component>(e)...);
			}
		}

		template<typename... Component>
		decltype(auto) GetComponent(const Entity& e) const
		{
			AE_PROFILE_FUNCTION();
			AE_CORE_ASSERT(IsValid(e), "Invalid Entity provided to Registry"); 

			if constexpr(sizeof...(Component) == 1)
			{
				return (Assure<Component>().Get(e), ...);
			}
			else
			{
				return std::forward_as_tuple(GetComponent<Component>(e), ...);
			}
		}

		//clears the components passed
		template<typename... Component>
		void Clear()
		{
			AE_PROFILE_FUNCTION();
			if constexpr(sizeof...(Component) == 0)
			{
				ForEach([this](const auto entity) 
					{
						this->DeleteEntity(entity);
					});
			}
			else
			{
				auto lambda = [this](auto&& pool)
					{
						pool.Remove(*this, pool.SparseSet<Entity>::begin(), pool.SparseSet<Entity>::end());
					};
				(lambda(Assure<Component>()), ...);
			}
		}

		//function signature should be void(const Entity)
		template<typename Func>
		void ForEach(Func function)
		{
			AE_PROFILE_FUNCTION();
			static_assert(std::is_invocable_v<Func, Entity>);
			for (Entity e : m_entities)
			{
				function(e);
			}
		}

		template<typename Component>
		auto OnCreate()
		{
			return Assure<Component>().OnCreate();
		}

		template<typename Component>
		auto OnDestroy()
		{
			return Assure<Component>().OnDestroy();
		}

		template<typename... Component, typename... Exclude>
		View<Entity, TypeList<Exclude...>, Component...> GetView(TypeList<Exclude...> = {})
		{
			static_assert(sizeof... (Component) > 0);
			//decay simplifies the type ex: (T[])& would map to T*
			return { Assure<std::decay_t<Component>>()..., Assure<Exclude>()... };
		}

		template<typename... Component, typename... Exclude>
		View<Entity, TypeList<Exclude...>, Component...> GetView(TypeList<Exclude...> = {})const
		{
			//assert at least one of the components is const
			static_assert(std::conjunction_v<std::is_const_v<Component>...>);
			//remove const from this and call the other GetView function
			return const_cast<Registry*>(this)->GetView<Component...>(ExcludeList<Exclude...>);
		}

		template<typename... Owned, typename... Get, typename... Exclude>
		Group<Entity, TypeList<Exclude...>, TypeList<Get...>, Owned...> GetGroup(TypeList<Get...>, 
			TypeList<Exclude...> = {})
		{
			AE_PROFILE_FUNCTION();
			static_assert(sizeof...(Owned) + sizeof...(Get) > 0);
			static_assert(sizeof...(Owned) + sizeof...(Get) + sizeof...(Exclude) > 1);

			using HandlerType = GroupHandler<TypeList<Exclude...>, TypeList<std::decay_t<Get>...>, std::decay_t<Owned>...>;
			const auto pools = std::forward_as_tuple(Assure<std::decay_t<Owned>>()..., Assure<std::decay_t<Get>>()...);
			constexpr auto size = sizeof...(Owned) + sizeof...(Get) + sizeof...(Exclude);
			HandlerType* handler = nullptr;

			//try to find a group handler if there was one previously created which is valid for the types provided
			{
				auto it = std::find_if(m_groups.begin(), m_groups.end(), [size](const auto& groupData) 
					{
					return groupData.size == size
						&& (groupData.owned(TypeInfo<std::decay_t<Owned>>::ID()) && ...)
						&& (groupData.get(TypeInfo<std::decay_t<Get>>::ID()) && ...)
						&& (groupData.exclude(TypeInfo<Exclude>::ID()) && ...);
					});

				if (it != m_groups.end())
				{
					handler = static_cast<HandlerType*>((*it).handler.Get());
				}
			}


			//if we cannot find a valid group handler previously created we create one and add it to the ADynArr of GroupData
			if (handler == nullptr)
			{
				//create the group data and add it to the ADynArr of GroupData
				AUniqueRef<void, void(void*)> ptr = AUniqueRef<void, void(void*)>(new HandlerType{}, [](void* instance) 
					{ delete static_cast<HandlerType*>(instance); });
				
				GroupData candidate = {
					size, ptr,
					[](const unsigned int type) { return ((type == TypeInfo<std::decay_t<Owned>>::ID()) || ...); },
					[](const unsigned int type) { return ((type == TypeInfo<std::decay_t<Get>>::ID()) || ...); },
					[](const unsigned int type) { return ((type == TypeInfo<Exclude>::ID()) || ...); }
				};

				handler = static_cast<HandlerType*>(candidate.handler.Get());

				if constexpr(sizeof...(Owned) == 0)
				{
					m_groups.AddLast(std::move(candidate));
				}
				else
				{
					AE_CORE_ASSERT(std::all_of(m_groups.begin(), m_groups.end(), [size](const auto& data) 
						{
							//uses bool as an unsigned int
							const auto overlapping = (0u + ... + data.owned(TypeInfo<std::decay_t<Owned>>::ID()));
							const auto sz = overlapping + (0u + ... + data.owned(TypeInfo<std::decay_t<Get>>::ID()))
								+ (0u + ... + data.owned(TypeInfo<Exclude>::ID()));

							return !overlapping || ((sz == size) || (sz == data.size));
						}), "");

					const auto next = std::find_if_not(m_groups.begin(), m_groups.end(), [size](const auto& data) 
						{
							return !(0u + ... + data.owned(TypeInfo<std::decay_t<Owned>>::ID())) || (size > (data.size));
						});

					const auto prev = std::find_if_not(m_groups.begin(), m_groups.end(), [size](const auto& data)
						{
							return !(0u + ... + data.owned(TypeInfo<std::decay_t<Owned>>::ID()));
						});

					m_groups.Insert(std::move(candidate), next);
				}

				((std::get<PoolHandler<std::decay_t<Owned>>&>(pools).super 
					= std::max<size_t>(std::get<PoolHandler<std::decay_t<Owned>>&>(pools).super, size)), ...);

				//links the MaybeValidIf & DiscardIf so that a group is re-evaluated and updated when different 
				//component types are being created and destroyed


				(OnCreate<std::decay_t<Owned>>().AddDelegate(ADelegate<void(Registry<Entity>&, const Entity)>()
					.BindFunction<&HandlerType::MaybeValidIf<std::decay_t<Owned>>>(handler)), ...);
				(OnCreate<std::decay_t<Get>>().AddDelegate(ADelegate<void(Registry<Entity>&, const Entity)>()
					.BindFunction<&HandlerType::MaybeValidIf<std::decay_t<Get>>>(handler)), ...);
				(OnDestroy<Exclude>().AddDelegate(ADelegate<void(Registry<Entity>&, const Entity)>()
					.BindFunction<&HandlerType::CheckValidityOnComponentDestroyed<Exclude>>(handler)), ...);

				(OnDestroy<std::decay_t<Owned>>().AddDelegate(ADelegate<void(Registry<Entity>&, const Entity)>()
					.BindFunction<&HandlerType::CheckValidityOnComponentDestroyed<std::decay_t<Owned>>>(handler)), ...);
				(OnDestroy<std::decay_t<Get>>().AddDelegate(ADelegate<void(Registry<Entity>&, const Entity)>()
					.BindFunction<&HandlerType::CheckValidityOnComponentDestroyed<std::decay_t<Get>>>(handler)), ...);
				(OnCreate<Exclude>().AddDelegate(ADelegate<void(Registry<Entity>&, const Entity)>()
					.BindFunction<&HandlerType::DiscardIf>(handler)), ...);

				if constexpr(sizeof...(Owned) == 0)
				{
					//add the entities owned & observed to the handler's sparse set
					for (const auto entity : GetView<Owned..., Get...>(TypeList<Exclude...>{}))
					{
						handler->current.Add(entity);
					}
				}
				else
				{
					auto* first = std::get<0>(pools).GetData();
					auto* last = first + std::get<0>(pools).GetCount();

					//compact owned components towards the start of the array by calling MaybeValidIf function
					for(; first != last; first++)
					{
						handler->template MaybeValidIf<std::tuple_element_t<0, std::tuple<std::decay_t<Owned>...>>>(*this, *first);
					}
				}
			}

			if constexpr(sizeof...(Owned) == 0)
			{
				//the group does not own any component so we create a non-owning group
				return { handler->current, std::get<PoolHandler<std::decay_t<Get>>&>(pools)... };
			}
			else
			{
				//the group owns components so we create an owning group
				return { handler->current, std::get<PoolHandler<std::decay_t<Owned>>&>(pools)..., 
					std::get<PoolHandler<std::decay_t<Get>>&>(pools)... };
			}
		}

		//const version of the GetGroup function
		template<typename... Owned, typename... Get, typename... Exclude>
		Group<Entity, TypeList<Exclude...>, TypeList<Get...>, Owned...> GetGroup(TypeList<Get...>, 
			TypeList<Exclude...> = {}) const 
		{
			static_assert(std::conjunction_v<std::is_const<Owned>..., std::is_const<Get>...>);
			return const_cast<Registry<Entity>*>(this)->GetGroup<Owned...>(get<Get...>, exclude<Exclude...>);
		}

		//GetGroup function but without observed components (only owned)
		template<typename... Owned, typename... Exclude>
		Group<Entity, TypeList<Exclude...>, TypeList<>, Owned...> GetGroup(TypeList<Exclude...> = {})
		{
			return GetGroup<Owned...>(TypeList<>{}, exclude<Exclude...>);
		}

		//const version of the GetGroup function with only owned components
		template<typename... Owned, typename... Exclude>
		Group<Entity, TypeList<Exclude...>, TypeList<>, Owned...> GetGroup(TypeList<Exclude...> = {}) const
		{
			static_assert(std::conjunction_v<std::is_const<Owned>...>);
			return const_cast<Registry<Entity>*>(this)->GetGroup<Owned...>(exclude<Exclude...>);
		}

	private:

		template<typename Component>
		class PoolHandler final : public Storage<Entity, Component>
		{
		public:
			static_assert(std::is_same_v<Component, std::decay_t<Component>>);

			//used when managing groups
			size_t super;

			/*returns a Sink for the respective events, the listeners 
			  will be called by the registry (the update function will 
			  have it's own function to trigger the update)
			*/
			auto OnCreate() { return Sink<void(Registry<Entity>&, const Entity)>(m_create); }
			auto OnDestroy() { return Sink<void(Registry<Entity>&, const Entity)>(m_destroy); }

			template<typename... Args>
			decltype(auto) Emplace(Registry<Entity>& owner, const Entity& e, Args... args)
			{
				AE_PROFILE_FUNCTION();

				if constexpr (std::is_same_v<AReference<CallbackComponent>, Component>)
				{
					auto comp = Storage<Entity, Component>::Emplace(e, std::forward<Args>(args)...);
					m_create.CallDelagates(owner, e);
					return comp;
				}
				else
				{
					auto& comp = Storage<Entity, Component>::Emplace(e, std::forward<Args>(args)...);
					m_create.CallDelagates(owner, e);
					return comp;
				}

			}

			void Remove(Registry<Entity>& owner, const Entity& e)
			{
				m_destroy(owner, e);
				auto& list = Storage<Entity, Component>::Remove(e);
			}

			template<typename Comp>
			void RemoveComponent(Registry<Entity>& owner, const Entity& e)
			{	
				m_destroy(owner, e);
				Storage<Entity, Component>::Remove(e);				
			}

			void RemoveComponent(Registry<Entity>& owner, const Entity& e, const Component& comp)
			{
				Storage<Entity, Component>::RemoveComponent(e, comp);

				if constexpr (std::is_base_of_v<CallbackComponent, Component>)
				{
					comp.OnDestroy();
				}

				m_destroy(owner, e);
			}


		private:
			SignalHandler<void(Registry<Entity>&, const Entity)> m_create;
			SignalHandler<void(Registry<Entity>&, const Entity)> m_destroy;
		};

		template<typename...>
		struct GroupHandler;

		template<typename... Exclude, typename... Get, typename... Owned>
		struct GroupHandler<TypeList<Exclude...>, TypeList<Get...>, Owned...>
		{
			static_assert(std::conjunction_v<std::is_same<Owned, std::decay_t<Owned>>..., 
				std::is_same<Get, std::decay_t<Get>>..., std::is_same<Exclude, std::decay_t<Exclude>>...>);

			std::conditional_t<sizeof...(Owned) == 0, ASparseSet<Entity>, size_t> current{};


			/*calls MaybeValidIf or DiscardIf depending on whether the component destroyed 
			  if the entity provided has no component of the destroyed type
			  
			  this functions is used to filter out the updating of groups when a component was 
			  destroyed on an entity but that entity still has other components of the same type

			  This functions is to be called when a component was destroyed
			*/
			template<typename Component>
			void CheckValidityOnComponentDestroyed(Registry<Entity>& owner, const Entity e)
			{
				//excluded component destroyed
				if constexpr ((std::is_same_v<Component, Exclude> || ...))
				{
					MaybeValidIf<Component>(owner, e);
				}
				else 
				{
					DiscardIf(owner, e);
				}
			}

			/*manages the order of the entity/component pair to compact the data managed 
			  by groups to optimize the iteration of them

			  this function will be called when a component of type Component is created (the entity is passed as argument)
			  
			  if the component is to be excluded it will be called when the excluded component is destroyed to see 
			  if the entity is now valid
			*/
			template<typename Component>
			void MaybeValidIf(Registry<Entity>& owner, const Entity e)
			{
				AE_PROFILE_FUNCTION();
				//assert we have decayed types
				static_assert(std::disjunction_v <std::is_same<Owned, 
					std::decay_t<Owned>>..., std::is_same<Get, std::decay_t<Get>>..., 
					std::is_same<Exclude, std::decay_t<Exclude>>...>);

				const auto pools = std::forward_as_tuple(owner.Assure<Owned>()...);

				const auto& isValid = ((std::is_same_v<Component, Owned> || std::get<PoolHandler<Owned>&>(pools).Contains(e)) && ...)
					&& ((std::is_same_v<Component, Get> || owner.Assure<Get>().Contains(e)) && ...)
					&& ((std::is_same_v<Component, Exclude> || !owner.Assure<Exclude>().Contains(e)) && ...);

				if constexpr(sizeof...(Owned) == 0)
				{
					if (isValid && !current.Contains(e))
					{
						current.Emplace(e);
					}
				}
				else
				{
					if (isValid && !(std::get<0>(pools).GetIndex(e) < current))
					{
						const auto pos = current++;
						(std::get<PoolHandler<Owned>&>(pools).Swap(std::get<PoolHandler<Owned>&>(pools).GetData()[pos], e), ...);
					}
				}
			}
		
			/*manages the order of the entity/component pair to compact the data managed
			  by groups to optimize the iteration of them

			  this function will be called when a component is destroyed (the entity is passed as argument)

			  if the component is to be excluded this function will be called when the excluded component is created to see
			  if the entity is now invalid
			*/
			void DiscardIf(Registry<Entity>& owner, const Entity e)
			{
				AE_PROFILE_FUNCTION();
				if constexpr (sizeof...(Owned) == 0)
				{
					if (current.Contains(e))
					{
						current.Remove(e);
					}
				}
				else
				{
					const auto pools = std::forward_as_tuple(owner.Assure<Owned>()...);
					if (std::get<0>(pools).Contains(e) && std::get<0>(pools).GetIndex(e) < current)
					{
						const auto pos = --current;
						(std::get<PoolHandler<Owned>&>(pools).Swap(
							std::get<PoolHandler<Owned>&>(pools).GetData()[pos], e), ...);
					}
				}
			}
		};

		struct GroupData
		{
			size_t size;

			//takes a void pointer and a void(*)(void*) function used to delete group handler
			AUniqueRef<void, void(void*)> handler;
			bool (*owned)(const unsigned int);
			bool (*get)(const unsigned int);
			bool (*exclude)(const unsigned int);

			bool operator==(const GroupData& other) const
			{
				return size == other.size && handler == other.handler;
			}

			bool operator!=(const GroupData& other) const
			{
				return !(*this == other);
			}
		};

		template<typename Component>
		PoolHandler<Component>& Assure()
		{
			AE_PROFILE_FUNCTION();
			//check if the component provided already has an index
			if constexpr(HasIndex<Component>::value)
			{
				//make sure that the index fits in the pool list (pool list can be cleared)
				unsigned int index = IndexProvider<Component>::GetIndex();
				if (!(index < m_pools.GetCount()))
				{
					//if not enough space for the index we reserve that space
					m_pools.Reserve(index + 1);
					m_pools.Insert(PoolData(), (size_t)index);
				}

				PoolData& data = m_pools[index];
				
				//if the pool is uninitialized we created the pool 
				//(pool list can be cleared &/or default value with pool being nullptr)
				if (data.pool == nullptr)
				{
					data.index = index;
					data.pool = AUniqueRef<PoolHandler<Component>>::Create();
					data.remove = [](ASparseSet<Entity>& pool, Registry<Entity>& owner, const Entity e)
					{
						static_cast<PoolHandler<Component>&>(pool).Remove(owner, e);
					};
				}

				return static_cast<PoolHandler<Component>&>(*data.pool);
			}
			else
			{
				SparseSet<Entity>* pool = nullptr;

				//check if there is a previously made pool for the component (pool list can be cleared)
				for (PoolData& data : m_pools)
				{
					if (data.index == IndexProvider<Component>::GetIndex())
					{
						pool = data.pool.Get();
						break;
					}
				}

				//if no pool has been made we create it
				if (pool == nullptr)
				{
					AUniqueRef<SparseSet<Entity>> ptr = AUniqueRef<SparseSet<Entity>>::Create();
					unsigned int index = IndexProvider<Component>::GetIndex();
					m_pools.EmplaceBack(PoolData{ index,
						ptr, 
						[](SparseSet<Entity> & pool, Registry<Entity> & owner, const Entity e)
							{
								static_cast<PoolHandler<Component>&>(pool).Remove(owner, e);
							}
						});
					pool = m_pools[index].pool.Get();
				}

				return static_cast<PoolHandler<Component>&>(*pool);
			}
		}

		template<typename Component>
		const PoolHandler<Component>& Assure() const
		{
			return (const_cast<Registry<Entity>*>(this))->template Assure<Component>();
		}

		/* removes all the components associated with the provided entity
		*/
		void RemoveAllComponents(const Entity e)
		{
			AE_PROFILE_FUNCTION();
			for (PoolData& data : m_pools)
			{
				if (data.pool != nullptr && data.pool->Contains(e))
				{
					data.remove(*data.pool, *this, e);
				}
			}
		}

		//stores the Storage of different component types (one type for each obj) 
		//without referencing the component to store it inside an arrayList
		struct PoolData
		{
			unsigned int index;

			//the different PoolHandler types for different Components
			AUniqueRef<ASparseSet<Entity>> pool;

			void (*remove)(ASparseSet<Entity>&, Registry<Entity>&, const Entity) {};

			bool operator==(const PoolData& other) const 
			{
				return index == other.index;
			}

			bool operator!=(const PoolData& other) const
			{
				return !(*this == other);
			}
		};

		ADoublyLinkedList<GroupData> m_groups;
		ADynArr<PoolData> m_pools;
		ADynArr<Entity> m_entities;
		ASinglyLinkedList<Entity> m_destroyedPos;
	};
}