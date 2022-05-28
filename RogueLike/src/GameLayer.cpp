#include "GameLayer.h"
#include "Scripts/BoardManager.h"
#include "Scripts/PlayerController.h"

#include <AstralEngine/EntryPoint.h>

namespace RogueLike
{
	GameLayer* GameLayer::s_instance;

	GameLayer::GameLayer() : m_scene(AReference<Scene>::Create(false)) { s_instance = this; }

	void GameLayer::OnAttach()
	{
		RemoveCameraControls();
		SetupTextures();
		SetupBoard();
		SetupPlayer();
	}

	void GameLayer::OnUpdate()
	{
		m_scene->OnUpdate();
	}

	AEntity GameLayer::GetPlayer()
	{
		return s_instance->m_player;
	}

	const AReference<Texture2D>& GameLayer::GetBlockTexture()
	{
		return s_instance->m_blockTexture;
	}

	const AReference<Texture2D>& GameLayer::GetGoalFlagTexture()
	{
		return s_instance->m_goalFlagTexture;
	}

	const AReference<Texture2D>& GameLayer::GetPlayerTexture()
	{
		return s_instance->m_playerTexture;
	}

	const AReference<Texture2D>& GameLayer::GetEnemyTexture()
	{
		return s_instance->m_enemyTexture;
	}

	bool GameLayer::OnEvent(AEvent& e)
	{
		if (e.GetType() == AEventType::WindowResize)
		{
			WindowResizeEvent& resize = (WindowResizeEvent&)e;
			m_scene->OnViewportResize(resize.GetWidth(), resize.GetHeight());
		}
		return false;
	}

	AEntity GameLayer::CreateBlockEntity()
	{
		AEntity block = m_scene->CreateAEntity();
		block.EmplaceComponent<SpriteRenderer>(m_blockTexture);
		block.GetTransform().SetParent(m_environment);
		block.SetName("Block");
		return block;
	}

	void GameLayer::SetupTextures()
	{
		m_goalFlagTexture = Texture2D::Create("assets/textures/goalFlag.png");
		m_blockTexture = Texture2D::Create("assets/textures/brickSprite.png");
		m_playerTexture = Texture2D::Create("assets/textures/player.png");
		m_enemyTexture = Texture2D::Create("assets/textures/enemy.png");
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

	void GameLayer::SetupPlayer()
	{
		m_player = m_scene->CreateAEntity();
		m_player.SetName("Player");
		m_player.GetTransform().SetParent(m_boardManager);
		m_player.GetTransform().scale.x = -1.0f;
		m_player.EmplaceComponent<SpriteRenderer>(m_playerTexture);
		m_player.EmplaceComponent<PlayerController>();
		m_player.EmplaceComponent<BoardMoveable>();
	}

	void GameLayer::RemoveCameraControls()
	{
		//hack to by-pass ECS will need to be modified in the future
		AEntity camera = AEntity((BaseEntity)0, m_scene.Get());
		Camera& cam = camera.GetComponent<Camera>();
		cam.primary = false;
		camera.Destroy();
		m_cam = m_scene->CreateAEntity();
		m_cam.EmplaceComponent<Camera>(cam).primary = true;
		m_cam.GetTransform().position = Vector3(-0.5f, -0.5f, - 1.0f);
	}

	RogueLikeGame::RogueLikeGame() : Application("Goomba Run") { AttachLayer(new GameLayer()); }
}

AstralEngine::Application* CreateApp()
{
	return new RogueLike::RogueLikeGame();
}