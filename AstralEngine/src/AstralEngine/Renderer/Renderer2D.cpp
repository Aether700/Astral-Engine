#include "aepch.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderCommand.h"
#include "AstralEngine/ECS/Components.h"
#include "AstralEngine/UI/UICore.h"
#include "AstralEngine/Core/Application.h"

namespace AstralEngine
{
	Renderer2DStatistics Renderer2D::s_stats = Renderer2DStatistics();

	struct QuadVertex
	{
		Vector3 position;
		Vector4 color;
		Vector2 texCoord;
		float textureIndex = -1.0f;
		float tillingFactor = 1.0f;
		float ignoresCamPos = 0.0f;
	};

	struct Renderer2DData
	{
		AReference<VertexBuffer> vertexBuffer;
		AReference<IndexBuffer> indexBuffer;
		AReference<Shader> shader;

		Vector4 quadPositions[4];

		unsigned int quadIndexCount = 0;

		QuadVertex* quadVertexBuffer = nullptr;
		QuadVertex* quadVertexPtr = nullptr;

		std::array<AReference<Texture2D>, Renderer2D::s_maxTextureSlots> textureSlots;
		unsigned int textureSlotIndex = 1; //index 0 is the white texture
	};

	static Renderer2DData s_data = Renderer2DData();

	void Renderer2D::Init()
	{
		AE_PROFILE_FUNCTION();

		s_data.vertexBuffer = VertexBuffer::Create(sizeof(QuadVertex) * s_maxVertices);

		s_data.quadVertexBuffer = new QuadVertex[s_maxVertices];

		VertexBufferLayout layout =
		{
			{ ADataType::Float3, "a_position" },
			{ ADataType::Float4, "a_color" },
			{ ADataType::Float2, "a_texCoord" },
			{ ADataType::Float, "a_texIndex" },
			{ ADataType::Float, "a_tilingFactor" },
			{ ADataType::Float, "a_ignoreCamPos" }
		};

		s_data.vertexBuffer->SetLayout(layout);


		unsigned int* indices = new unsigned int[s_maxIndices];

		int offset = 0;

		for (int i = 0; i < s_maxIndices; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;

			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}


		s_data.indexBuffer = IndexBuffer::Create(indices, s_maxIndices);

		int samplers[s_maxTextureSlots];
		for (int i = 0; i < s_maxTextureSlots; i++)
		{
			samplers[i] = i;
		}

		s_data.shader = Shader::Create("assets/shaders/Shader2D.glsl");
		s_data.shader->Bind();
		s_data.shader->SetIntArray("u_texture", samplers, s_maxTextureSlots);

		unsigned int whiteTextureData = 0xffffffff;
		AReference<Texture2D> whiteTexture = Texture2D::Create(1, 1, &whiteTextureData, sizeof(unsigned int));
		s_data.textureSlots[0] = whiteTexture;

		s_data.quadPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_data.quadPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_data.quadVertexBuffer;
	}

	const Renderer2DStatistics& Renderer2D::GetStats()
	{
		return s_stats;
	}

	void Renderer2D::ResetStats()
	{
		s_stats.Reset();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		AE_PROFILE_FUNCTION();
		s_data.shader->Bind();
		s_data.shader->SetMat4("u_viewProjMatrix", camera.GetViewProjectionMatrix());

		StartBatch();
	}

	void Renderer2D::BeginScene(const RuntimeCamera& camera, const Transform& transform)
	{
		AE_PROFILE_FUNCTION();
		Mat4 viewProj = camera.GetProjectionMatrix() * transform.GetTransformMatrix().Inverse();

		s_data.shader->Bind();
		s_data.shader->SetMat4("u_viewProjMatrix", viewProj);

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		FlushBatch();
	}

	AReference<Texture2D> Renderer2D::GetDefaultTexture()
	{
		return s_data.textureSlots[0];
	}

	void Renderer2D::DrawQuad(const Vector2& position, const Vector2& size, const Vector4& color, bool ignoresCamPos)
	{
		DrawQuad((Vector3)position, size, color, ignoresCamPos);
	}
	
	void Renderer2D::DrawQuad(const Vector3& position, const Vector2& size, const Vector4& color, bool ignoreCamPos)
	{
		AE_PROFILE_FUNCTION();

		CheckBatchCapacity();
		UploadSimpleQuad(position, size, 0, 1.0f, color, ignoreCamPos);
	}

	void Renderer2D::DrawQuad(const Vector2& position, const Vector2& size, AReference<Texture2D> texture,
		float tileFactor, const Vector4& tintColor, bool ignoresCamPos)
	{
		DrawQuad((Vector3)position, size, texture, tileFactor, tintColor, ignoresCamPos);
	}

	void Renderer2D::DrawQuad(const Vector3& position, const Vector2& size, AReference<Texture2D> texture,
		float tileFactor, const Vector4& tintColor, bool ignoresCamPos)
	{
		AE_PROFILE_FUNCTION();

		CheckBatchCapacity();
		int textureIndex = GetTextureIndex(texture);
		UploadSimpleQuad(position, size, (float)textureIndex, tileFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const Vector3& position, const Vector2& size, AReference<SubTexture2D> subTexture,
		float tileFactor, const Vector4& tintColor, bool ignoresCamPos)
	{
		AE_PROFILE_FUNCTION();

		CheckBatchCapacity();
		int textureIndex = GetTextureIndex(subTexture->GetTexture());

		const Vector2 textureCoordinates[] = {
			{ subTexture->GetMin().x, subTexture->GetMin().y },
			{ subTexture->GetMax().x, subTexture->GetMin().y },
			{ subTexture->GetMax().x, subTexture->GetMax().y },
			{ subTexture->GetMin().x, subTexture->GetMax().y }
		};

		UploadSimpleQuad(position, size, (float)textureIndex, tileFactor, tintColor, textureCoordinates, ignoresCamPos);
	}

	void Renderer2D::DrawQuad(const Mat4& transform, const Vector4& color, bool ignoresCamPos)
	{
		CheckBatchCapacity();
		
		const Vector2 texCoord[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		constexpr float textureIndex = 0.0f;
		constexpr float tileFactor = 1.0f;

		for (size_t i = 0; i < 4; i++)
		{
			s_data.quadVertexPtr->position = transform * s_data.quadPositions[i];
			s_data.quadVertexPtr->color = color;
			s_data.quadVertexPtr->texCoord = texCoord[i];
			s_data.quadVertexPtr->textureIndex = textureIndex;
			s_data.quadVertexPtr->tillingFactor = tileFactor;
			s_data.quadVertexPtr->ignoresCamPos = ignoresCamPos ? 1.0f : 0.0f;
			s_data.quadVertexPtr++;
		}

		s_data.quadIndexCount += 6;

		s_stats.numIndices += 6;
		s_stats.numVertices += 4;
	}

	void Renderer2D::DrawQuad(const Mat4& transform, AReference<Texture2D> texture, 
		float tileFactor, const Vector4& tintColor, bool ignoresCamPos)
	{
		CheckBatchCapacity();

		const Vector2 texCoord[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		float textureIndex = (float)GetTextureIndex(texture);

		for (size_t i = 0; i < 4; i++)
		{
			s_data.quadVertexPtr->position = transform * s_data.quadPositions[i];
			s_data.quadVertexPtr->color = tintColor;
			s_data.quadVertexPtr->texCoord = texCoord[i];
			s_data.quadVertexPtr->textureIndex = textureIndex;
			s_data.quadVertexPtr->tillingFactor = tileFactor;
			s_data.quadVertexPtr->ignoresCamPos = ignoresCamPos ? 1.0f : 0.0f;
			s_data.quadVertexPtr++;
		}

		s_data.quadIndexCount += 6;

		s_stats.numIndices += 6;
		s_stats.numVertices += 4;
	}

	void Renderer2D::DrawQuad(const Mat4& transform, AReference<SubTexture2D> subTexture, 
		float tileFactor, const Vector4& tintColor, bool ignoresCamPos)
	{
		CheckBatchCapacity();

		const Vector2 texCoord[] = {
			{ subTexture->GetMin().x, subTexture->GetMin().y },
			{ subTexture->GetMax().x, subTexture->GetMin().y },
			{ subTexture->GetMax().x, subTexture->GetMax().y },
			{ subTexture->GetMin().x, subTexture->GetMax().y }
		};

		float textureIndex = (float)GetTextureIndex(subTexture->GetTexture());

		for (size_t i = 0; i < 4; i++)
		{
			s_data.quadVertexPtr->position = transform * s_data.quadPositions[i];
			s_data.quadVertexPtr->color = tintColor;
			s_data.quadVertexPtr->texCoord = texCoord[i];
			s_data.quadVertexPtr->textureIndex = textureIndex;
			s_data.quadVertexPtr->tillingFactor = tileFactor;
			s_data.quadVertexPtr->ignoresCamPos = ignoresCamPos ? 1.0f : 0.0f;
			s_data.quadVertexPtr++;
		}

		s_data.quadIndexCount += 6;

		s_stats.numIndices += 6;
		s_stats.numVertices += 4;
	}



	void Renderer2D::DrawRotatedQuad(const Vector2& position, float rotation, const Vector2& size, const Vector4& color)
	{
		DrawRotatedQuad((Vector3)position, rotation, size, color);
	}
	
	void Renderer2D::DrawRotatedQuad(const Vector3& position, float rotation, const Vector2& size, const Vector4& color)
	{
		AE_PROFILE_FUNCTION();

		Mat4 transform = Mat4::Identity();
		transform.Translate(position);
		transform.Rotate(rotation, Vector3{ 0.0f, 0.0f, 1.0f });
		transform.Scale(size);

		DrawQuad(transform, color);
	}
	
	void Renderer2D::DrawRotatedQuad(const Vector3& position, float rotation, const Vector2& size,
		const SpriteRenderer& sprite)
	{
		if (sprite.GetSprite() == nullptr)
		{
			DrawRotatedQuad(position, rotation, size, sprite.GetColor());
		}
		else
		{
			DrawRotatedQuad(position, rotation, size, sprite.GetSprite(), 1.0f, sprite.GetColor());
		}
	}

	void Renderer2D::DrawRotatedQuad(const Transform& transform, const SpriteRenderer& sprite)
	{
		DrawRotatedQuad(transform.GetLocalPosition(), transform.GetRotation().EulerAngles().z, 
			transform.GetScale(), sprite);
	}

	void Renderer2D::DrawRotatedQuad(const Vector2& position, float rotation, const Vector2& size, AReference<Texture2D> texture,
		float tileFactor, const Vector4& tintColor)
	{
		DrawRotatedQuad((Vector3)position, rotation, size, texture, tileFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const Vector3& position, float rotation, const Vector2& size, AReference<Texture2D> texture,
		float tileFactor, const Vector4& tintColor)
	{
		AE_PROFILE_FUNCTION();
		
		Mat4 transform = Mat4::Identity();
		transform.Translate(position);
		transform.Rotate(rotation, Vector3{ 0.0f, 0.0f, 1.0f });
		transform.Scale(size);

		DrawQuad(transform, texture, tileFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const Vector3& position, float rotation, const Vector2& size, 
		AReference<SubTexture2D> texture, float tileFactor, const Vector4& tintColor)
	{
		AE_PROFILE_FUNCTION();

		Mat4 transform = Mat4::Identity();
		transform.Translate(position);
		transform.Rotate(rotation, Vector3{ 0.0f, 0.0f, 1.0f });
		transform.Scale(size);

		DrawQuad(transform, texture, tileFactor, tintColor);
	}


	void Renderer2D::DrawUIElement(const UIElement& element, const Vector4& color)
	{
		DrawQuad(element.GetWorldPos(), Vector2(element.GetWorldWidth(),
			element.GetWorldHeight()), color, true);
	}

	void Renderer2D::StartBatch()
	{
		AE_PROFILE_FUNCTION();

		s_data.quadVertexPtr = s_data.quadVertexBuffer;
		s_data.quadIndexCount = 0;
		s_data.textureSlotIndex = 1;
	}

	void Renderer2D::FlushBatch()
	{
		AE_PROFILE_FUNCTION();

		unsigned int dataSize = (unsigned int)((unsigned char*)s_data.quadVertexPtr 
			- (unsigned char*)s_data.quadVertexBuffer);
		s_data.vertexBuffer->SetData(s_data.quadVertexBuffer, dataSize);

		for (unsigned int i = 0; i < s_data.textureSlotIndex; i++)
		{
			s_data.textureSlots[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(s_data.indexBuffer, s_data.quadIndexCount);
		s_stats.numDrawCalls++;
	}

	void Renderer2D::CheckBatchCapacity()
	{
		if (s_data.quadIndexCount == s_maxIndices)
		{
			FlushBatch();
			StartBatch();
		}
	}

	int Renderer2D::GetTextureIndex(const AReference<Texture2D>& texture)
	{
		AE_PROFILE_FUNCTION();

		int textureIndex = 0;

		for (unsigned int i = 1; i < s_data.textureSlotIndex; i++)
		{
			if (*texture == *s_data.textureSlots[i])
			{
				textureIndex = (int)i;
				break;
			}
		}

		if (textureIndex == 0)
		{
			if (s_data.textureSlotIndex == s_maxTextureSlots)
			{
				FlushBatch();
				StartBatch();
			}

			s_data.textureSlots[s_data.textureSlotIndex] = texture;
			textureIndex = s_data.textureSlotIndex;
			s_data.textureSlotIndex++;
		}

		return textureIndex;
	}

	void Renderer2D::UploadSimpleQuad(const Vector3& position, const Vector2& size, float textureIndex,
		float tileFactor, const Vector4& tintColor, bool ignoresCamPos)
	{
		AE_PROFILE_FUNCTION();


		Vector2 texCoord[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		UploadSimpleQuad(position, size, textureIndex, tileFactor, tintColor, texCoord, ignoresCamPos);
	}

	void Renderer2D::UploadSimpleQuad(const Vector3& position, const Vector2& size, float textureIndex,
		float tileFactor, const Vector4& tintColor, const Vector2* textureCoords, bool ignoresCamPos)
	{
		AE_PROFILE_FUNCTION();

		const Vector3 positions[] = {
			{ position.x - (size.x / 2.0f), position.y - (size.y / 2.0f), position.z },
			{ position.x + (size.x / 2.0f), position.y - (size.y / 2.0f), position.z },
			{ position.x + (size.x / 2.0f), position.y + (size.y / 2.0f), position.z },
			{ position.x - (size.x / 2.0f), position.y + (size.y / 2.0f), position.z }
		};

		for (size_t i = 0; i < 4; i++)
		{
			s_data.quadVertexPtr->position = positions[i];
			s_data.quadVertexPtr->color = tintColor;
			s_data.quadVertexPtr->texCoord = textureCoords[i];
			s_data.quadVertexPtr->textureIndex = textureIndex;
			s_data.quadVertexPtr->tillingFactor = tileFactor;
			s_data.quadVertexPtr->ignoresCamPos = ignoresCamPos ? 1.0f : 0.0f;
			s_data.quadVertexPtr++;
		}
		
		s_data.quadIndexCount += 6;
	}

}