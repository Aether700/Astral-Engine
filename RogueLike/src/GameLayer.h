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
		void OnDetach() override;

	private:
		AReference<Scene> m_scene;
	};


	class RogueLikeGame : public Application
	{
	public:
		RogueLikeGame();
	};
}