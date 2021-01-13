#pragma once
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Math/AMath.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "OrthographicCamera.h"

namespace AstralEngine
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera); //to do take scene parameters
		static void EndScene();

		static void Submit(const AReference<VertexBuffer>& vertexArray, const AReference<IndexBuffer>& indexBuffer, 
			const AReference<Shader>& shader, Mat4 transform = Mat4::Identity());

		inline static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }

	private:

		struct SceneData
		{
			Mat4 viewProjMatrix;
		};

		static AReference<SceneData> s_sceneData;

	};
}