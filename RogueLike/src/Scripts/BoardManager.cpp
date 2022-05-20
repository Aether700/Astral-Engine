#include "BoardManager.h"
#include "../GameLayer.h"
#include "BoardMoveable.h"
#include "EnemyAI.h"

namespace RogueLike
{
	BoardManager* BoardManager::s_instance = nullptr;

	void BoardManager::OnCreate()
	{
		s_instance = this;
	}

	void BoardManager::OnStart()
	{
		m_level = 1;
		InitializeBoard();
		entity.GetTransform().position = Vector3(1, 1, 0);
	}

	AEntity BoardManager::GetCell(size_t i, size_t j)
	{
		return s_instance->m_board[i + s_size * j];
	}

	void BoardManager::SetCell(AEntity e, size_t i, size_t j)
	{
		s_instance->m_board[i + s_size * j] = e;
	}

	size_t BoardManager::GetBoardSize()
	{
		return s_size;
	}

	Vector2Int BoardManager::FindEntityOnBoard(AEntity e)
	{
		for(size_t i = 0; i < s_size; i++)
		{
			for(size_t j = 0; j < s_size; j++)
			{
				if (GetCell(i, j) == e)
				{
					return Vector2Int(i, j);
				}
			}
		}
		return Vector2Int(-1, -1);
	}

	void BoardManager::RegenerateBoard()
	{
		s_instance->ClearBoard();
		s_instance->GenerateBlocks();
		s_instance->GenerateEnemies();
	}

	void BoardManager::ResetLevel()
	{
		s_instance->m_level = 1;
	}

	void BoardManager::IncrementLevel()
	{
		s_instance->m_level++;
	}

	void BoardManager::ClearBoard()
	{
		for (size_t i = 0; i < s_size * s_size; i++)
		{
			if (m_board[i].IsValid() && (m_board[i].GetName() == "Block" || m_board[i].GetName() == "Enemy"))
			{
				Destroy(m_board[i]);
			}
			m_board[i] = NullEntity;
		}
		SetCell(m_goal, s_size - 1, s_size - 1);
	}

	void BoardManager::InitializeBoard()
	{
		m_goal = CreateAEntity();
		m_goal.GetTransform().SetParent(entity);
		m_goal.EmplaceComponent<SpriteRenderer>(GameLayer::GetGoalFlagTexture());
		SetCell(m_goal, s_size - 1, s_size - 1);
		m_goal.GetTransform().position = Vector3(s_size - 1, s_size - 1, 0);
		GenerateBlocks();
		GenerateEnemies();
	}
	
	void BoardManager::GenerateBlocks()
	{
		// generate random blocks
		size_t numBlocks = GetNumBlocks();

		for (size_t i = 0; i < numBlocks; i++)
		{
			Vector2Int coords = GetRandomBlockCoord();
			AEntity currCoord = GetCell(coords.x, coords.y);
			while (currCoord != NullEntity)
			{
				coords = GetRandomBlockCoord();
				currCoord = GetCell(coords.x, coords.y);
			}
			AE_INFO("(%d, %d)", coords.x, coords.y);
			SetCell(CreateInnerBlock(coords), coords.x, coords.y);
		}
	}

	size_t BoardManager::GetNumBlocks() const
	{
		return Math::Min((size_t)(5.0f * Math::Log(m_level) + 5.0f), 17);
	}

	Vector2Int BoardManager::GetRandomBlockCoord() const
	{
		return Vector2Int((Random::GetInt() % (s_size - 2)) + 1, (Random::GetInt() % (s_size - 2)) + 1);
	}

	void BoardManager::GenerateEnemies()
	{
		size_t numEnemies = GetNumEnemies();

		for (size_t i = 0; i < numEnemies; i++)
		{
			Vector2Int coords = GetRandomEnemyCoord();

			while (BoardManager::GetCell(coords.x, coords.y) != NullEntity || coords == Vector2Int::Zero() 
				|| coords.x < 3 || coords.y < 3)
			{
				coords = GetRandomEnemyCoord();
			}
			SetCell(CreateEnemy(coords), coords.x, coords.y);
		}
	}
	
	size_t BoardManager::GetNumEnemies() const
	{
		if (m_level < 3)
		{
			return 1;
		}
		else if (m_level < 8) 
		{
			return 2;
		}
		return 3;
	}

	Vector2Int BoardManager::GetRandomEnemyCoord() const
	{
		return Vector2Int(Random::GetInt() % s_size, Random::GetInt() % s_size);
	}

	AEntity BoardManager::CreateInnerBlock(const Vector2Int& coords) const
	{
		AEntity innerBlock = CreateAEntity();
		innerBlock.GetTransform().position = Vector3(coords.x, coords.y, 0);
		innerBlock.GetTransform().SetParent(entity);
		innerBlock.EmplaceComponent<SpriteRenderer>(GameLayer::GetBlockTexture());
		innerBlock.SetName("Block");
		return innerBlock;
	}

	AEntity BoardManager::CreateEnemy(const Vector2Int& coords) const
	{
		AEntity enemy = CreateAEntity();
		enemy.SetName("Enemy");
		enemy.GetTransform().SetParent(entity);
		BoardMoveable& move = enemy.EmplaceComponent<BoardMoveable>();
		move.Set(coords);
		enemy.EmplaceComponent<SpriteRenderer>(GameLayer::GetEnemyTexture());
		EnemyAI& ai = enemy.EmplaceComponent<EnemyAI>();
		ai.SaveStartPos();
		return enemy;
	}
}