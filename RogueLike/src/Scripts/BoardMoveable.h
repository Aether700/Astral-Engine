#pragma once
#include "BoardManager.h"

#include <AstralEngine.h>

namespace RogueLike
{
	class BoardMoveable : public NativeScript
	{
	public:
		bool Move(const Vector2Int& dir);
		void Set(const Vector2Int& coord);

		const Vector2Int& GetCoords() const { return m_boardCoords; }

	private:
		Vector2Int m_boardCoords;
	};
}