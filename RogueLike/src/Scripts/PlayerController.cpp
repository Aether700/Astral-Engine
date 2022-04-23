#include "PlayerController.h"

namespace RogueLike
{
	void PlayerController::OnStart()
	{
		GetComponent<BoardMoveable>().Set(Vector2Int::Zero());
	}

	void PlayerController::OnUpdate()
	{
		m_hasMoved = false;
		Vector2Int dir = Vector2Int::Zero();

		if (Input::GetKeyDown(KeyCode::W))
		{
			dir = Vector2Int::Up();
		}

		if (Input::GetKeyDown(KeyCode::S))
		{
			if (dir != Vector2Int::Zero())
			{
				return;
			}
			dir = Vector2Int::Down();
		}

		if (Input::GetKeyDown(KeyCode::D))
		{
			if (dir != Vector2Int::Zero())
			{
				return;
			}
			dir = Vector2Int::Right();
		}

		if (Input::GetKeyDown(KeyCode::A))
		{
			if (dir != Vector2Int::Zero())
			{
				return;
			}
			dir = Vector2Int::Left();
		}

		if (dir == Vector2Int::Right())
		{
			GetTransform().scale.x = -Math::Abs(GetTransform().scale.x);
		}
		else if (dir == Vector2Int::Left())
		{
			GetTransform().scale.x = Math::Abs(GetTransform().scale.x);
		}
		
		if (dir != Vector2Int::Zero())
		{
			if (GetComponent<BoardMoveable>().Move(dir))
			{
				m_hasMoved = true;
				
				if (GetTransform().position == Vector3(BoardManager::GetBoardSize() - 1, BoardManager::GetBoardSize() - 1, 0))
				{
					ChangeLevel();
				}
			}
		}
	}

	bool PlayerController::HasMoved() const { return m_hasMoved; }

	void PlayerController::ChangeLevel()
	{
		BoardManager::RegenerateBoard();
		GetComponent<BoardMoveable>().Set(Vector2Int::Zero());
	}

}