#pragma once
#include "BoardMoveable.h"
#include "EnemyAI.h"

namespace RogueLike
{
	class PlayerController : public NativeScript
	{
	public:
		void OnStart() override;
		void OnUpdate() override;
		bool HasMoved() const;

		void RegisterEnemy(EnemyAI& enemy);

		void KillPlayer();

	private:
		void ChangeLevel();
		void ResetLevel();
		void RegenerateLevel();

		void ResetGame();
		
		void RespawnPlayerAfterDeath();

		bool m_hasMoved;
		bool m_isDead;

		unsigned int m_skipCount = 3;
		unsigned int m_currSkipCount;

		float m_deathTime = 3.0f;
		float m_currDeathTimer = 0.0f;
		ADynArr<AEntity> m_enemies;
	};
}