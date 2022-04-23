#include "BoardMoveable.h"

namespace RogueLike
{
	bool BoardMoveable::Move(const Vector2Int& dir)
	{
		Vector2Int newPos = m_boardCoords + dir;
		if (newPos.x < 0 || newPos.x >= BoardManager::GetBoardSize()
			|| newPos.y < 0 || newPos.y >= BoardManager::GetBoardSize())
		{
			return false;
		}
		
		AEntity targetCell = BoardManager::GetCell(newPos.x, newPos.y);

		if (targetCell == NullEntity || targetCell.GetName() == "Player")
		{
			BoardManager::SetCell(NullEntity, m_boardCoords.x, m_boardCoords.y);
			m_boardCoords = newPos;
			BoardManager::SetCell(entity, m_boardCoords.x, m_boardCoords.y);
			GetTransform().position = Vector3(m_boardCoords.x, m_boardCoords.y, 0.0f);
			return true;
		}
		return false;
	}

	void BoardMoveable::Set(const Vector2Int& coord)
	{
		m_boardCoords = coord;
		BoardManager::SetCell(entity, m_boardCoords.x, m_boardCoords.y);
		GetTransform().position = Vector3(m_boardCoords.x, m_boardCoords.y, 0.0f);
	}
}