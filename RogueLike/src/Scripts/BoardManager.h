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

	private:

		void ClearBoard();
		void InitializeBoard();
		size_t GetNumBlocks() const;
		Vector2Int GetRandomBlockCoord() const;

		void GenerateBlocks();
		void GenerateEnemies();

		AEntity CreateInnerBlock(const Vector2Int& coords) const;
		AEntity CreateEnemy(const Vector2Int& coords) const;

		static constexpr size_t s_size = 7;
		static BoardManager* s_instance;


		size_t m_level;
		AEntity m_board[s_size * s_size];
	};
}