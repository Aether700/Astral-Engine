#include "GameLayer.h"
#include "Scripts/BoardManager.h"

#include <AstralEngine/EntryPoint.h>

namespace RogueLike
{
	GameLayer* GameLayer::s_instance;

	GameLayer::GameLayer() : m_scene(AReference<Scene>::Create(false)) { s_instance = this; }

	void GameLayer::OnAttach()
	{
		m_goalFlagTexture = Texture2D::Create("assets/textures/goalFlag.png");
		m_blockTexture = Texture2D::Create("assets/textures/brickSprite.png");
		m_playerTexture = Texture2D::Create("assets/textures/player.png");
		
		SetupBoard();
		m_player = m_scene->CreateAEntity();
		m_player.EmplaceComponent<SpriteRenderer>(m_playerTexture);
		m_player.GetTransform().SetParent(m_boardManager);
	}

	void GameLayer::OnUpdate()
	{
		m_scene->OnUpdate();
	}

	void GameLayer::OnDetach()
	{

	}

	const AReference<Texture2D>& GameLayer::GetBlockTexture() const
	{
		return m_blockTexture;
	}

	const AReference<Texture2D>& GameLayer::GetGoalFlagTexture() const
	{
		return m_goalFlagTexture;
	}

	const AReference<Texture2D>& GameLayer::GetPlayerTexture() const
	{
		return m_playerTexture;
	}

	GameLayer* GameLayer::GetGameLayer() { return s_instance; }

	AEntity GameLayer::CreateBlockEntity()
	{
		AEntity block = m_scene->CreateAEntity();
		block.EmplaceComponent<SpriteRenderer>(m_blockTexture);
		block.GetTransform().SetParent(m_environment);
		return block;
	}

	void GameLayer::SetupBoard()
	{
		m_environment = m_scene->CreateAEntity();
		m_boardManager = m_scene->CreateAEntity();
		m_boardManager.EmplaceComponent<BoardManager>();
		m_boardManager.GetTransform().SetParent(m_environment);

		size_t boardSize = BoardManager::GetBoardSize();
		float offset = (float)(boardSize + 2.0f) / 2.0f;

		m_environment.GetTransform().position = Vector3(-offset, -offset, 0.0f);

		for (size_t j = 0; j < boardSize + 2; j++)
		{
			for(size_t i = 0; i < boardSize + 2; i++)
			{
				if (j == 0 || j == boardSize + 1 || i == 0 || i == boardSize + 1)
				{
					AEntity currBlock = CreateBlockEntity();
					currBlock.GetTransform().position = Vector3((float)i, (float)j, 0);
				}
			}
		}
	}

	RogueLikeGame::RogueLikeGame() { AttachLayer(new GameLayer()); }
}

AstralEngine::Application* CreateApp()
{
	return new RogueLike::RogueLikeGame();
}