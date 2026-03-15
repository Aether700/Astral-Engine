#pragma once
#include "Scene.h"
#include "CallbackComponentList.h"
#include "ColliderList.h"

namespace AstralEngine
{
	class CallbackComponent;
	class CallbackList;
	
	class ComponentModifiedHelper
	{
	public:
		template<typename Component>
		static void OnComponentAdded(BaseEntity e, Scene* scene)
		{
			CheckComponentListAdded<CallbackComponent, CallbackList, Component>(e, scene);
			CheckComponentListAdded<Collider2D, ColliderList, Component>(e, scene);
		}

		template<typename Component>
		static void OnComponentRemoved(BaseEntity e, Scene* scene)
		{
			CheckComponentListRemove<CallbackComponent, CallbackList, Component>(e, scene);
			CheckComponentListRemove<Collider2D, ColliderList, Component>(e, scene);
		}

	private:
		template<typename TargetType, typename TargetList, typename Component>
		static void CheckComponentListAdded(BaseEntity e, Scene* scene)
		{
			if constexpr (std::is_base_of_v<TargetType, Component>)
			{
				if (scene->m_registry.HasComponent<TargetList>(e))
				{
					scene->m_registry.GetComponent<TargetList>(e).Add<Component>(e, scene);
				}
				else
				{
					scene->m_registry.EmplaceComponent<TargetList>(e).Add<Component>(e, scene);
				}
			}
		}

		template<typename TargetType, typename TargetList, typename Component>
		static void CheckComponentListRemove(BaseEntity e, Scene* scene)
		{
			if constexpr (std::is_base_of_v<TargetType, Component>)
			{
				AE_CORE_ASSERT(scene->m_registry.HasComponent<TargetList>(e));
				TargetList* list;
				list = scene->m_registry.GetComponent<TargetList>(e);
				list->Remove<Component>();

				if (list->IsEmpty())
				{
					scene->m_registry.RemoveComponent<TargetList>(e);
				}
			}
		}
	};
}