#include "BoardManager.h"
#include "../GameLayer.h"

namespace RogueLike
{
	void BoardManager::OnStart()
	{
		m_level = 1;
		InitializeBoard();
		entity.GetTransform().position = Vector3(1, 1, 0);
	}

	AEntity BoardManager::GetCell(size_t i, size_t j) const
	{
		return m_board[i + s_size * j];
	}

	void BoardManager::SetCell(AEntity e, size_t i, size_t j)
	{
		m_board[i + s_size * j] = e;
	}

	size_t BoardManager::GetBoardSize()
	{
		return s_size;
	}

	void BoardManager::InitializeBoard()
	{
		AEntity goal = CreateAEntity();
		goal.GetTransform().SetParent(entity);
		goal.EmplaceComponent<SpriteRenderer>(GameLayer::GetGameLayer()->GetGoalFlagTexture());
		goal.GetTransform().position = Vector3(s_size - 1, s_size - 1, 0);

		// generate random blocks
		size_t numBlocks = GetNumBlocks();

		for(size_t i = 0; i < numBlocks; i++)
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

		return innerBlock;
	}
}