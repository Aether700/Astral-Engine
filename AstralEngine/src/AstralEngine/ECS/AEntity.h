#pragma once
#include "ECS Core/Registry.h"
#include "Scene.h"

namespace AstralEngine
{
	class Transform;
	class CallbackComponent;
	class CallbackList;

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
				
				if (HasComponent<CallbackList>())
				{
					GetComponent<CallbackList>().AddCallback(&comp);
				}
				else
				{
					CallbackList& list = EmplaceComponent<CallbackList>();
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
			m_scene->m_registry.EmplaceComponent<Component>(*this, c);
			if constexpr (std::is_base_of_v<CallbackComponent, Component>)
			{
				if (HasComponent<CallbackList>())
				{
					GetComponent<CallbackList>().AddCallback(&c);
				}
				else
				{
					CallbackList& list = EmplaceComponent<CallbackList>();
					list.AddCallback(&c);
				}
			}
		}

		template<typename Component>
		void RemoveComponent()
		{
			if constexpr (std::is_base_of_v<CallbackComponent, Component>)
			{
				AE_CORE_ASSERT(HasComponent<CallbackList>(),
					"CallbackListComponent was not added to entity with a CallbackComponent");
				CallbackList* list;
				list = &GetComponent<CallbackList>();
				list->RemoveCallback(&GetComponent<Component>());

				if (list->IsEmpty()) 
				{
					RemoveComponent<CallbackList>();
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
				CallbackList* list;
				list = GetComponent<CallbackList>();
				list->RemoveCallback(&comp);

				if (list->IsEmpty())
				{
					RemoveComponent<CallbackList>();
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

		const std::string& GetName() const;
		void SetName(const std::string& newName);

		Transform& GetTransform();
		const Transform& GetTransform() const;

		bool IsActive() const;
		void SetActive(bool val);

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