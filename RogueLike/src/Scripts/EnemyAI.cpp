#include "EnemyAI.h"
#include "../GameLayer.h"
#include "BoardMoveable.h"
#include "PlayerController.h"

namespace RogueLike
{
	void EnemyAI::OnCreate()
	{
		m_player = GameLayer::GetPlayer();
		m_player.GetComponent<PlayerController>().RegisterEnemy(*this);
	}

	void EnemyAI::OnLateUpdate()
	{
		if (!m_player.GetComponent<PlayerController>().HasMoved())
		{
			return;
		}

		BoardMoveable& moveable = GetComponent<BoardMoveable>();

		Vector2Int desiredMove = ComputeDesiredMove();
		Vector2Int resultPos = desiredMove + moveable.GetCoords();
		if (BoardManager::GetCell(resultPos.x, resultPos.y) == m_player)
		{
			m_player.GetComponent<PlayerController>().KillPlayer();
		}

		moveable.Move(desiredMove);
	}

	AEntity EnemyAI::GetEntity() const { return entity; }

	void EnemyAI::SaveStartPos()
	{
		m_startPos = GetComponent<BoardMoveable>().GetCoords();
	}

	void EnemyAI::ResetPosition()
	{
		GetComponent<BoardMoveable>().Set(m_startPos);
	}

	Vector2Int EnemyAI::ComputeDesiredMove()
	{
		BoardMoveable& playerMoveable = m_player.GetComponent<BoardMoveable>();
		Vector2Int playerCoords = playerMoveable.GetCoords();

		BoardMoveable& ownMoveable = GetComponent<BoardMoveable>();
		Vector2Int currCoords = ownMoveable.GetCoords();
		Vector2Int moveNeeded = playerCoords - currCoords;

		Vector2Int moveDir;

		if (Math::Abs(moveNeeded.x) > Math::Abs(moveNeeded.y))
		{
			// try to move x first
			moveDir = MoveX(currCoords, moveNeeded);

			if (moveDir == Vector2Int::Zero())
			{
				moveDir = MoveY(currCoords, moveNeeded);
			}
		}
		else
		{
			// try to move x first
			moveDir = MoveY(currCoords, moveNeeded);

			if (moveDir == Vector2Int::Zero())
			{
				moveDir = MoveX(currCoords, moveNeeded);
			}
		}

		return moveDir;
	}

	Vector2Int EnemyAI::MoveX(const Vector2Int& currCoords,	const Vector2Int& moveNeeded)
	{
		if (moveNeeded.x == 0)
		{
			return Vector2Int::Zero();
		}
		int xComp = moveNeeded.x > 0 ? 1 : -1;
		Vector2Int moveDir =  Vector2Int(xComp, 0);
		
		if (IsValidMove(currCoords + moveDir))
		{
			return moveDir;
		}
		return Vector2Int::Zero();
	}

	Vector2Int EnemyAI::MoveY(const Vector2Int& currCoords, const Vector2Int& moveNeeded)
	{
		if (moveNeeded.y == 0)
		{
			return Vector2Int::Zero();
		}
		int yComp = moveNeeded.y > 0 ? 1 : -1;
		Vector2Int moveDir = Vector2Int(0, yComp);
		if (IsValidMove(currCoords + moveDir))
		{
			return moveDir;
		}
		return Vector2Int::Zero();
	}

	bool EnemyAI::IsValidMove(const Vector2Int& moveResult)
	{
		return BoardManager::GetCell(moveResult.x, moveResult.y) == NullEntity
			|| BoardManager::GetCell(moveResult.x, moveResult.y) == m_player;
	}
}