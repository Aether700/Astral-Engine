#pragma once
#include "AstralEngine.h"

namespace RogueLike
{
	using namespace AstralEngine;

	class BoardManager : public NativeScript
	{
	public:
		void OnStart() override;

		AEntity GetCell(size_t i, size_t j) const;
		void SetCell(AEntity e, size_t i, size_t j);

		static size_t GetBoardSize();

	private:
		void InitializeBoard();
		size_t GetNumBlocks() const;
		Vector2Int GetRandomBlockCoord() const;

		AEntity CreateInnerBlock(const Vector2Int& coords) const;

		static constexpr size_t s_size = 7;

		size_t m_level;
		AEntity m_board[s_size * s_size];
	};
}