#include "BoardManager.h"
#include "../GameLayer.h"

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
		// generate random blocks
		size_t numBlocks = s_instance->GetNumBlocks();

		for (size_t i = 0; i < numBlocks; i++)
		{
			Vector2Int coords = s_instance->GetRandomBlockCoord();
			AEntity currCoord = s_instance->GetCell(coords.x, coords.y);
			while (currCoord != NullEntity)
			{
				coords = s_instance->GetRandomBlockCoord();
				currCoord = s_instance->GetCell(coords.x, coords.y);
			}
			AE_INFO("(%d, %d)", coords.x, coords.y);
			s_instance->SetCell(s_instance->CreateInnerBlock(coords), coords.x, coords.y);
		}
	}

	void BoardManager::ClearBoard()
	{
		for (size_t i = 0; i < s_size * s_size; i++)
		{
			if (m_board[i].IsValid() && m_board[i].GetName() == "Block")
			{
				Destroy(m_board[i]);
			}
			m_board[i] = NullEntity;
		}
	}

	void BoardManager::InitializeBoard()
	{
		AEntity goal = CreateAEntity();
		goal.GetTransform().SetParent(entity);
		goal.EmplaceComponent<SpriteRenderer>(GameLayer::GetGameLayer()->GetGoalFlagTexture());
		goal.GetTransform().position = Vector3(s_size - 1, s_size - 1, 0);
		RegenerateBoard();
	}

	size_t BoardManager::GetNumBlocks() const
	{
		return (size_t)(5.0f * Math::Log(m_level) + 10.0f);
	}

	Vector2Int BoardManager::GetRandomBlockCoord() const
	{
		return Vector2Int((Random::GetInt() % (s_size - 2)) + 1, (Random::GetInt() % (s_size - 2)) + 1);
	}

	AEntity BoardManager::CreateInnerBlock(const Vector2Int& coords) const
	{
		AEntity innerBlock = CreateAEntity();
		innerBlock.GetTransform().position = Vector3(coords.x, coords.y, 0);
		innerBlock.GetTransform().SetParent(entity);
		innerBlock.EmplaceComponent<SpriteRenderer>(GameLayer::GetGameLayer()->GetBlockTexture());
		innerBlock.SetName("Block");
		return innerBlock;
	}
}