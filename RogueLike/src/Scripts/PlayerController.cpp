#include "PlayerController.h"

namespace RogueLike
{
	void PlayerController::OnStart()
	{
		GetComponent<BoardMoveable>().Set(Vector2Int::Zero());
		m_isDead = false;
	}

	void PlayerController::OnUpdate()
	{
		m_hasMoved = false;


		if (m_isDead)
		{
			if (Input::GetAnyKeyDown())
			{
				m_currSkipCount++;

				if (m_currSkipCount >= m_skipCount)
				{
					RespawnPlayerAfterDeath();
				}
			}

			if (m_currDeathTimer < m_deathTime)
			{
				m_currDeathTimer += Time::GetDeltaTime();
				return;
			}
			else
			{
				RespawnPlayerAfterDeath();
			}
		}

		if (Input::GetKeyDown(KeyCode::Backspace))
		{
			ResetGame();
		}
		else if (Input::GetKeyDown(KeyCode::R))
		{
			ResetLevel();
		}
		else if (Input::GetKeyDown(KeyCode::Enter))
		{
			RegenerateLevel();
		}


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

	void PlayerController::RegisterEnemy(EnemyAI& enemy)
	{
		m_enemies.Add(enemy.GetEntity());
	}

	void PlayerController::KillPlayer()
	{
		m_isDead = true;
		m_currDeathTimer = 0.0f;
		GetComponent<SpriteRenderer>().SetActive(false);
	}

	void PlayerController::ChangeLevel()
	{
		AE_INFO("changing level");
		m_hasMoved = false;
		m_enemies.Clear();
		BoardManager::IncrementLevel();
		BoardManager::RegenerateBoard();
		GetComponent<BoardMoveable>().Set(Vector2Int::Zero());
	}
	
	void PlayerController::RegenerateLevel()
	{
		m_hasMoved = false;
		m_enemies.Clear();
		BoardManager::RegenerateBoard();
		GetComponent<BoardMoveable>().Set(Vector2Int::Zero());
	}

	void PlayerController::ResetGame()
	{
		m_hasMoved = false;
		m_enemies.Clear();
		BoardManager::ResetLevel();
		BoardManager::RegenerateBoard();
		GetComponent<BoardMoveable>().Set(Vector2Int::Zero());
	}

	void PlayerController::RespawnPlayerAfterDeath()
	{
		m_currSkipCount = 0;
		m_isDead = false;
		GetComponent<SpriteRenderer>().SetActive(true);
		ResetLevel();
	}

	void PlayerController::ResetLevel()
	{
		{
			BoardMoveable& move = GetComponent<BoardMoveable>();
			Vector2Int coords = move.GetCoords();
			BoardManager::SetCell(NullEntity, coords.x, coords.y);
			move.Set(Vector2Int::Zero());
		}

		for(AEntity& e : m_enemies)
		{
			e.GetComponent<EnemyAI>().ResetPosition();
		}
	}

}