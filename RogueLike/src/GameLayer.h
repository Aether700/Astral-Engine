#pragma once
#include <AstralEngine.h>

namespace RogueLike
{
	using namespace AstralEngine;
	
	class GameLayer : public Layer
	{
	public:
		GameLayer();

		void OnAttach() override;
		void OnUpdate() override;

		const AReference<Texture2D>& GetBlockTexture() const;
		const AReference<Texture2D>& GetGoalFlagTexture() const;
		const AReference<Texture2D>& GetPlayerTexture() const;
		
		static GameLayer* GetGameLayer();

		bool OnEvent(AEvent& e) override;

		AEntity m_environment;
	private:
		static GameLayer* s_instance;

		AEntity CreateBlockEntity();
		void SetupTextures();
		void SetupBoard();
		void SetupPlayer();

		void RemoveCameraControls();

		AReference<Scene> m_scene;
		AReference<Texture2D> m_blockTexture;
		AReference<Texture2D> m_goalFlagTexture;
		AReference<Texture2D> m_playerTexture;

		//entities
		AEntity m_boardManager;
		AEntity m_player;
		AEntity m_cam;
	};


	class RogueLikeGame : public Application
	{
	public:
		RogueLikeGame();
	};
}