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

		static size_t GetNumTextureSlots();
		static size_t GetMaxNumVertices();
		static size_t GetMaxNumIndices();

		//defaults to triangles
		static void DrawIndexed(const AReference<IndexBuffer>& indexBuffer, unsigned int count = 0);
		static void DrawIndexed(RenderingPrimitive primitive, const AReference<IndexBuffer>& indexBuffer, 
			unsigned int count = 0);

		static void DrawInstancedIndexed(const AReference<IndexBuffer>& indexBuffer, unsigned int instanceAmount, unsigned int count = 0);
	private:
		static RenderAPI* s_api;
	};
}