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
				AReference<Component> comp = AReference<Component>::Create(std::forward<Args>(args)...);
				m_scene->m_registry.EmplaceComponent<AReference<CallbackComponent>>(*this, comp);
				if constexpr (std::is_base_of_v<NativeScript, Component>)
				{
					comp->SetEntity(*this);
				}

				comp->OnCreate();

				return *comp.Get();
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
		}

		template<typename Component>
		void RemoveComponent()
		{
			if constexpr(std::is_base_of_v<CallbackComponent, Component>)
			{
				m_scene->m_registry.RemoveComponent<Component>(*this);
			}
			else
			{
				m_scene->m_registry.RemoveComponent<Component>(*this);
			}
		}

		template<typename Component>
		void RemoveComponent(const Component& comp)
		{
			if constexpr (std::is_base_of_v<CallbackComponent, Component>)
			{
				m_scene->m_registry.RemoveComponent<AReference<CallbackComponent>>(*this, comp);
			}
			else
			{
				m_scene->m_registry.RemoveComponent<Component>(*this, comp);
			}
		}

		template<typename... Component>
		decltype(auto) GetComponent()
		{
			//if we have at least one NativeScript
			if constexpr ((std::is_base_of_v<CallbackComponent, Component> || ...))
			{
				if constexpr (sizeof...(Component) == 1)
				{
					ADoublyLinkedList<AKeyElementPair<unsigned int, AReference<CallbackComponent>>>& list
						= m_scene->m_registry.GetComponentList<AReference<CallbackComponent>>(*this);

					for (AKeyElementPair<unsigned int, AReference<CallbackComponent>>& pair : list)
					{
						if (IndexProvider<Component...>::GetIndex() == pair.GetKey()) 
						{
							return (*(Component*)pair.GetElement().Get(), ...);
						}
					}

					AE_CORE_ERROR("CallbackComponent not found");
				}
				else
				{
					return std::forward_as_tuple(this->GetComponent<Component>()...);
				}
			}
			else
			{
				return m_scene->m_registry.GetComponent<Component...>(*this);
			}
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
				if constexpr (std::is_base_of_v<CallbackComponent, Component...>)
				{
					auto& list = m_scene->m_registry.GetComponentList<AReference<CallbackComponent>>(*this);
					for (auto& pair : list)
					{
						if (pair.GetKey() == TypeInfo<Component...>::ID())
						{
							return true;
						}
					}
					return false;
				}
				else
				{
					return (m_scene->m_registry.HasComponent<Component>(*this) && ...);
				}				
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