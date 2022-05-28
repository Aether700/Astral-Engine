#pragma once
#include "ECS Core/Registry.h"


namespace AstralEngine
{

	class Scene
	{
		friend class AEntity;
	public:
		Scene(bool rotation = true);

		AEntity CreateAEntity();

		void DestroyAEntity(AEntity e);

		void OnUpdate();
		void OnViewportResize(unsigned int width, unsigned int height);

	private:

		void CallOnStart();
		void CallOnUpdate();
		void CallOnLateUpdate();
		void DestroyEntitiesToDestroy();

		Registry<BaseEntity> m_registry;
		ADynArr<AEntity> m_entitiesToDestroy;
		unsigned int m_viewportWidth;
		unsigned int m_viewportHeight;
	};
}