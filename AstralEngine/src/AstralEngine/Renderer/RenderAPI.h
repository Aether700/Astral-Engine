#pragma once
#include "AstralEngine/Data Struct/AReference.h"
#include "IndexBuffer.h"

namespace AstralEngine
{
	enum class RenderingPrimitive
	{
		Triangles
	};

	class RenderAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL
		};

		virtual void Init() = 0;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;
		virtual void Clear() = 0;
		virtual Vector4 GetClearColor() = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;

		virtual size_t GetNumTextureSlots() = 0;
		virtual size_t GetMaxNumVertices() = 0;
		virtual size_t GetMaxNumIndices() = 0;

		virtual void EnableBlending(bool enabled) = 0;

		virtual void DrawIndexed(const AReference<IndexBuffer>& indexBuffer) = 0;
		virtual void DrawIndexed(const AReference<IndexBuffer>& indexBuffer, unsigned int count) = 0;
		virtual void DrawIndexed(RenderingPrimitive primitive, const AReference<IndexBuffer>& indexBuffer, unsigned int count) = 0;

		virtual void DrawInstancedIndexed(const AReference<IndexBuffer>& indexBuffer, 
			unsigned int instanceAmount, unsigned int count) = 0;

		inline static API GetAPI() { return s_api; }

		static RenderAPI* Create();

	private:
		static API s_api;
	};
}