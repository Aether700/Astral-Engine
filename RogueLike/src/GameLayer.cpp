#include "GameLayer.h"

namespace RogueLike
{
	GameLayer::GameLayer() : m_scene(AReference<Scene>::Create(false)) { }

	void GameLayer::OnAttach()
	{
		AEntity background = m_scene->CreateAEntity();
		background.EmplaceComponent<SpriteRenderer>(0.2f, 0.2f, 0.2f, 1.0f);
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