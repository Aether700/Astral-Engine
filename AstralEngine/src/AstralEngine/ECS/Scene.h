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

		Registry<BaseEntity> m_registry;
		unsigned int m_viewportWidth;
		unsigned int m_viewportHeight;
	};
}