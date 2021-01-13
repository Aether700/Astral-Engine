#pragma once
#include "RenderAPI.h"
#include "VertexBuffer.h"
#include "AstralEngine/Math/AMath.h"

namespace AstralEngine
{
	class RenderCommand
	{
	public:
		static void Init();
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		static void Clear();
		static void SetClearColor(float r, float g, float b, float a);
		static void SetClearColor(const Vector4& color);

		static void DrawIndexed(const AReference<IndexBuffer>& indexBuffer, unsigned int count = 0);
	private:
		static RenderAPI* s_api;
	};
}