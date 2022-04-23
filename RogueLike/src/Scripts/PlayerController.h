#pragma once
#include "BoardMoveable.h"

namespace RogueLike
{
	class PlayerController : public NativeScript
	{
	public:
		void OnStart() override;
		void OnUpdate() override;
		bool HasMoved() const;

	private:
		void ChangeLevel();

		bool m_hasMoved;
	};
}