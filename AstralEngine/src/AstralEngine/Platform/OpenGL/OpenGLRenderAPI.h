#pragma once
#include "AstralEngine/Renderer/RenderAPI.h"

namespace AstralEngine
{
	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;
		virtual void Clear() override;
		virtual void SetClearColor(float r, float g, float b, float a) override;

		virtual void DrawIndexed(const AReference<IndexBuffer>& indexBuffer) override;
		virtual void DrawIndexed(const AReference<IndexBuffer>& indexBuffer, unsigned int count) override;
	};
}