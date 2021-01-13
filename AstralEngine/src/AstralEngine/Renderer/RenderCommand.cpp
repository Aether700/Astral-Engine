#include "aepch.h"
#include "RenderCommand.h"

namespace AstralEngine
{
	RenderAPI* RenderCommand::s_api = RenderAPI::Create();

	void RenderCommand::Init()
	{
		s_api->Init();
	}

	void RenderCommand::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		s_api->SetViewport(x, y, width, height);
	}

	void RenderCommand::Clear()
	{
		s_api->Clear(); 
	}
	
	void RenderCommand::SetClearColor(float r, float g, float b, float a) 
	{
		s_api->SetClearColor(r, g, b, a); 
	}
	
	void RenderCommand::SetClearColor(const Vector4& color) 
	{
		s_api->SetClearColor(color.r, color.g, color.b, color.a);
	}

	void RenderCommand::DrawIndexed(const AReference<IndexBuffer>& indexBuffer, unsigned int count)
	{
		s_api->DrawIndexed(indexBuffer, count);
	}
}