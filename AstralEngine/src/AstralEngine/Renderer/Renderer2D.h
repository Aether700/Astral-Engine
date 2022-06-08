#pragma once
#include "OrthographicCamera.h"
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Data Struct/AReference.h"
#include "Texture.h"


namespace AstralEngine
{
	class UIElement;
	class SpriteRenderer;
	class Transform;
	class RuntimeCamera;

	struct Renderer2DStatistics
	{
		unsigned int numDrawCalls = 0;
		unsigned int numVertices = 0;
		unsigned int numIndices = 0;

		void Reset()
		{
			numDrawCalls = 0;
			numVertices = 0;
			numIndices = 0;
		}
	};

	class Renderer2D
	{
		friend struct Renderer2DData;
	public:

		static void Init();
		static void Shutdown();
		
		static const Renderer2DStatistics& GetStats();
		static void ResetStats();

		static void BeginScene(const OrthographicCamera& camera);
		static void BeginScene(const RuntimeCamera& camera, const Transform& transform);
		static void EndScene();

		static AReference<Texture2D> GetDefaultTexture();

		//simple quads (no rotation)
		static void DrawQuad(const Vector2& position, const Vector2& size, const Vector4& color, bool ignoresCamPos = false);
		static void DrawQuad(const Vector3& position, const Vector2& size, const Vector4& color, bool ignoresCamPos = false);

		static void DrawQuad(const Vector2& position, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = {1, 1, 1, 1}, bool ignoresCamPos = false);
		static void DrawQuad(const Vector3& position, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 }, bool ignoresCamPos = false);
		static void DrawQuad(const Vector3& position, const Vector2& size, AReference<SubTexture2D> subTexture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 }, bool ignoresCamPos = false);

		static void DrawQuad(const Mat4& transform, const Vector4& color, bool ignoresCamPos = false);
		static void DrawQuad(const Mat4& transform, AReference<Texture2D> texture, 
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 }, bool ignoresCamPos = false);
		static void DrawQuad(const Mat4& transform, AReference<SubTexture2D> subTexture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 }, bool ignoresCamPos = false);

		//quads
		static void DrawRotatedQuad(const Vector2& position, float rotation, const Vector2& size, const Vector4& color);
		static void DrawRotatedQuad(const Vector3& position, float rotation, const Vector2& size, const Vector4& color);

		static void DrawRotatedQuad(const Vector3& position, float rotation, const Vector2& size, 
			const SpriteRenderer& sprite);

		static void DrawRotatedQuad(const Transform& transform, const SpriteRenderer& sprite);
		static void DrawRotatedQuad(const Vector2& position, float rotation, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 });
		static void DrawRotatedQuad(const Vector3& position, float rotation, const Vector2& size, AReference<Texture2D> texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 });
		static void DrawRotatedQuad(const Vector3& position, float rotation, const Vector2& size, 
			AReference<SubTexture2D> subTexture, float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 });

		//UI
		static void DrawUIElement(const UIElement& element, const Vector4& color);

	private:
		static void StartBatch();
		static void FlushBatch();
		static void CheckBatchCapacity();
		static int GetTextureIndex(const AReference<Texture2D>& texture);

		
		static void UploadSimpleQuad(const Vector3& position, const Vector2& size, float textureIndex,
			float tileFactor, const Vector4& tintColor, bool isUI = false);

		static void UploadSimpleQuad(const Vector3& position, const Vector2& size, float textureIndex,
			float tileFactor, const Vector4& tintColor, const Vector2* textureCoords, bool isUI = false);


		static const unsigned int s_maxQuads = 10000;
		static const unsigned int s_maxVertices = s_maxQuads * 4;
		static const unsigned int s_maxIndices = s_maxQuads * 6;
		static const unsigned int s_maxTextureSlots = 32;

		static Renderer2DStatistics s_stats;
	};
}