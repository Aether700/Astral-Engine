#pragma once
#include "AstralEngine.h"

namespace RogueLike
{
	using namespace AstralEngine;

	class BoardManager : public NativeScript
	{
	public:
		void OnCreate() override;
		void OnStart() override;

		static AEntity GetCell(size_t i, size_t j);
		static void SetCell(AEntity e, size_t i, size_t j);

		static size_t GetBoardSize();
		static Vector2Int FindEntityOnBoard(AEntity e);

		static void RegenerateBoard();
		static void ResetLevel();
		static void IncrementLevel();

	private:

		void ClearBoard();
		void InitializeBoard();
		void GenerateBlocks();
		size_t GetNumBlocks() const;
		Vector2Int GetRandomBlockCoord() const;
		
		void GenerateEnemies();
		size_t GetNumEnemies() const;
		Vector2Int GetRandomEnemyCoord() const;

		AEntity CreateInnerBlock(const Vector2Int& coords) const;
		AEntity CreateEnemy(const Vector2Int& coords) const;

		AEntity SetBlock(const Vector2Int& coords);
		AEntity SetEnemy(const Vector2Int& coords);

		static constexpr size_t s_size = 7;
		static BoardManager* s_instance;


		size_t m_level;
		AEntity m_goal;
		AEntity m_board[s_size * s_size];
		ADynArr<AEntity> m_blocks;
		ADynArr<AEntity> m_enemies;
	};
}