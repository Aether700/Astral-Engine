#pragma once
#include "AstralEngine.h"

namespace RogueLike
{
	using namespace AstralEngine;
	
	class EnemyAI : public NativeScript
	{
	public:
		void OnCreate();
		void OnLateUpdate() override;

		AEntity GetEntity() const;

		void SetStartPos(const Vector2Int& coords);
		void ResetPosition();

	private:
		//returns direction to move to this frame
		Vector2Int ComputeDesiredMove();

		//returns the move direction or zero vector if cannot move in that direction
		Vector2Int MoveX(const Vector2Int& currCoords, const Vector2Int& moveNeeded);
		Vector2Int MoveY(const Vector2Int& currCoords, const Vector2Int& moveNeeded);

		bool IsValidMove(const Vector2Int& moveResult);

		AEntity m_player;
		Vector2Int m_startPos;
	};
}