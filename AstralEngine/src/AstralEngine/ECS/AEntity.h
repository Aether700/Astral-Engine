#pragma once
#include "ECS Core/Registry.h"
#include "Scene.h"

namespace AstralEngine
{
	class AEntity
	{
		friend class NativeScript;
	public:
		AEntity() : m_id(Null), m_scene(nullptr) { }

		AEntity(BaseEntity entity, Scene* scene) : m_id(entity), m_scene(scene) { }

		AEntity(const AEntity& other) : m_id(other.m_id), m_scene(other.m_scene) { }

		template<typename Component, typename... Args>
		Component& EmplaceComponent(Args... args)
		{
			if constexpr(std::is_base_of_v<CallbackComponent, Component>)
			{
				Component& comp = m_scene->m_registry.EmplaceComponent<Component>(*this, std::forward<Args>(args)...);
				
				if (HasComponent<CallbackListComponent>())
				{
					GetComponent<CallbackListComponent>().AddCallback(&comp);
				}
				else
				{
					CallbackListComponent& list = EmplaceComponent<CallbackListComponent>();
					list.AddCallback(&comp);
				}

				if constexpr (std::is_base_of_v<NativeScript, Component>)
				{
					comp.SetEntity(*this);
				}

				comp.OnCreate();

				return comp;
			}
			else
			{
				return m_scene->m_registry.EmplaceComponent<Component>(*this, std::forward<Args>(args)...);
			}
		}

		template<typename Component>
		void AddComponent(const Component& c)
		{
			m_scene->m_registry.Add(*this, c);
			if constexpr (std::is_base_of_v<CallbackComponent, Component>)
			{
				if (HasComponent<CallbackListComponent>())
				{
					GetComponent<CallbackListComponent>().AddCallback(&c);
				}
				else
				{
					CallbackListComponent& list = EmplaceComponent<CallbackListComponent>();
					list.AddCallback(&c);
				}
			}
		}

		template<typename Component>
		void RemoveComponent()
		{
			if constexpr (std::is_base_of_v<CallbackComponent, Component>)
			{
				AE_CORE_ASSERT(HasComponent<CallbackListComponent>(),
					"CallbackListComponent was not added to entity with a CallbackComponent");
				CallbackListComponent* list;
				list = &GetComponent<CallbackListComponent>(); 
				list->RemoveCallback(&GetComponent<Component>());

				if (list->IsEmpty()) 
				{
					RemoveComponent<CallbackListComponent>();
				}
			}
			m_scene->m_registry.RemoveComponent<Component>(*this);
		}

		template<typename Component>
		void RemoveComponent(const Component& comp)
		{
			if constexpr (std::is_base_of_v<CallbackComponent, Component>)
			{
				AE_CORE_ASSERT(HasComponent<CallbackListComponent>(),
					"CallbackListComponent was not added to entity with a CallbackComponent");
				CallbackListComponent* list;
				list = GetComponent<CallbackListComponent>();
				list->RemoveCallback(&comp);

				if (list->IsEmpty())
				{
					RemoveComponent<CallbackListComponent>();
				}
			}
			m_scene->m_registry.RemoveComponent<Component>(*this, comp);
		}

		template<typename... Component>
		decltype(auto) GetComponent()
		{
			return m_scene->m_registry.GetComponent<Component...>(*this);
		}

		template<typename... Component>
		decltype(auto) GetComponent() const
		{
			return (const_cast<AEntity*>(this))->template GetComponent<Component...>();
		}

		template<typename... Component>
		bool HasComponent() const
		{
			if constexpr (sizeof... (Component) == 1)
			{
				return (m_scene->m_registry.HasComponent<Component>(*this) && ...);
			}
			else
			{
				return (HasComponent<Component>() && ...);
			}
		}

		//makes the entity invalid
		void Destroy()
		{
			m_scene->DestroyAEntity(*this);
		}

		BaseEntity GetID() const { return m_id; }

		operator BaseEntity() const
		{
			return m_id;
		}

		bool IsValid() const
		{
			if (m_scene == nullptr)
			{
				return false;
			}

			return m_scene->m_registry.IsValid(*this);
		}

		bool operator==(const AEntity& e) const { return m_id == e.m_id; }


	private:
		BaseEntity m_id;
		Scene* m_scene;
	};
}