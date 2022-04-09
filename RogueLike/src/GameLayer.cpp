#include "GameLayer.h"

namespace RogueLike
{
	GameLayer::GameLayer() : m_scene(AReference<Scene>::Create(false)) { }

	void GameLayer::OnAttach()
	{

	}

	void GameLayer::OnUpdate()
	{

	}

	void GameLayer::OnDetach()
	{

	}

	RogueLikeGame::RogueLikeGame() { AttachLayer(new GameLayer()); }

	AstralEngine::Application* CreateApp()
	{
		return new RogueLikeGame();
	}
}