#include "aepch.h"
#include "Renderer.h"
#include "Renderer2D.h"

namespace AstralEngine
{
	AReference<Renderer::SceneData> Renderer::s_sceneData = AReference<Renderer::SceneData>::Create();

	void Renderer::Init()
	{
		AE_PROFILE_FUNCTION();
		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		AE_PROFILE_FUNCTION();
		Renderer2D::Shutdown();
	}

	void Renderer::BeginScene(const OrthographicCamera& camera) 
	{
		AE_PROFILE_FUNCTION();
		s_sceneData->viewProjMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene() 
	{
	}

	void Renderer::Submit(const AReference<VertexBuffer>& vertexBuffer, const AReference<IndexBuffer>& indexBuffer,
		const AReference<Shader>& shader, Mat4 transform)
	{
		AE_PROFILE_FUNCTION();
		vertexBuffer->Bind();
		indexBuffer->Bind();
		shader->Bind();
		shader->SetMat4("u_viewProjMatrix", s_sceneData->viewProjMatrix);
		shader->SetMat4("u_transform", transform);

		RenderCommand::DrawIndexed(indexBuffer);
	}
}