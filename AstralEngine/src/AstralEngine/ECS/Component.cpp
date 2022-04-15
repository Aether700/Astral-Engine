#include "aepch.h"
#include "Components.h"

namespace AstralEngine
{
	// SpriteRenderer ///////////////////////////////////////////////////
	SpriteRenderer::SpriteRenderer()
		: m_color(1.0f, 1.0f, 1.0f, 1.0f),
		m_sprite(Renderer2D::GetDefaultTexture()) { }

	SpriteRenderer::SpriteRenderer(float r, float g, float b, float a)
		: m_color(r, g, b, a), m_sprite(Renderer2D::GetDefaultTexture())
	{
		//make sure color provided is a valid color
		AE_CORE_ASSERT(r <= 1.0f && r >= 0.0f
			&& g <= 1.0f && g >= 0.0f
			&& b <= 1.0f && b >= 0.0f
			&& a <= 1.0f && a >= 0.0f, "Invalid Color provided");
	}

	SpriteRenderer::SpriteRenderer(const Vector4& color)
		: m_color(color), m_sprite(Renderer2D::GetDefaultTexture())
	{
		//make sure color provided is a valid color
		AE_CORE_ASSERT(color.r <= 1.0f && color.r >= 0.0f
			&& color.g <= 1.0f && color.g >= 0.0f
			&& color.b <= 1.0f && color.b >= 0.0f
			&& color.a <= 1.0f && color.a >= 0.0f, "Invalid Color provided");
	}

	SpriteRenderer::SpriteRenderer(const AReference<Texture2D>& sprite)
		: m_color(1.0f, 1.0f, 1.0f, 1.0f), m_sprite(sprite)
	{
		AE_CORE_ASSERT(sprite != nullptr, "Invalid Sprite provided");
	}

	SpriteRenderer::SpriteRenderer(float r, float g, float b, float a, const AReference<Texture2D>& sprite)
		: m_color(r, g, b, a), m_sprite(sprite)
	{
		AE_CORE_ASSERT(sprite != nullptr, "Invalid Sprite provided");
		//make sure color provided is a valid color
		AE_CORE_ASSERT(r <= 1.0f && r >= 0.0f
			&& g <= 1.0f && g >= 0.0f
			&& b <= 1.0f && b >= 0.0f
			&& a <= 1.0f && a >= 0.0f, "Invalid Color provided");
	}

	SpriteRenderer::SpriteRenderer(const Vector4& color, const AReference<Texture2D>& sprite)
		: m_color(color), m_sprite(sprite)
	{
		AE_CORE_ASSERT(sprite != nullptr, "Invalid Sprite provided");
		//make sure color provided is a valid color
		AE_CORE_ASSERT(color.r <= 1.0f && color.r >= 0.0f
			&& color.g <= 1.0f && color.g >= 0.0f
			&& color.b <= 1.0f && color.b >= 0.0f
			&& color.a <= 1.0f && color.a >= 0.0f, "Invalid Color provided");
	}

	void SpriteRenderer::SetColor(float r, float g, float b, float a)
	{
		//make sure color provided is a valid color
		AE_CORE_ASSERT(r <= 1.0f && r >= 0.0f
			&& g <= 1.0f && g >= 0.0f
			&& b <= 1.0f && b >= 0.0f
			&& a <= 1.0f && a >= 0.0f, "Invalid Color provided");

		m_color = Vector4(r, g, b, a);
	}

	void SpriteRenderer::SetColor(Vector4 color)
	{
		//make sure color provided is a valid color
		AE_CORE_ASSERT(color.r <= 1.0f && color.r >= 0.0f
			&& color.g <= 1.0f && color.g >= 0.0f
			&& color.b <= 1.0f && color.b >= 0.0f
			&& color.a <= 1.0f && color.a >= 0.0f, "Invalid Color provided");

		m_color = color;
	}

	void SpriteRenderer::SetSprite(AReference<Texture2D> sprite)
	{
		AE_CORE_ASSERT(sprite != nullptr, "Invalid Sprite provided");
		m_sprite = sprite;
	}

	bool SpriteRenderer::operator==(const SpriteRenderer& other) const
	{
		return m_color == other.m_color && m_sprite == other.m_sprite;
	}

	bool SpriteRenderer::operator!=(const SpriteRenderer& other) const
	{
		return !(*this == other);
	}


	// Transform //////////////////////////////////////////////////////

	Transform::Transform() : scale(1.0f, 1.0f, 1.0f) { }
	Transform::Transform(Vector3 translation)
		: position(translation), scale(1.0f, 1.0f, 1.0f) { }

	Transform::Transform(Vector3 pos, Vector3 rotation, Vector3 scale)
		: position(pos), rotation(rotation), scale(scale) { }


	Mat4 Transform::GetTransformMatrix() const
	{
		Mat4 transformMatrix;
		if (rotation == Vector3::Zero())
		{
			transformMatrix = Mat4::Translate(Mat4::Identity(), position) * Mat4::Scale(Mat4::Identity(), scale);
		}
		else
		{
			Mat4 rotationMatrix = Mat4::Rotate(Mat4::Identity(), rotation.x, { 1, 0, 0 })
				* Mat4::Rotate(Mat4::Identity(), rotation.y, { 0, 1, 0 })
				* Mat4::Rotate(Mat4::Identity(), rotation.z, { 0, 0, 1 });
			transformMatrix = Mat4::Translate(Mat4::Identity(), position)
				* rotationMatrix * Mat4::Scale(Mat4::Identity(), scale);
		}

		if (m_parent.IsValid())
		{
			return m_parent.GetTransform().GetTransformMatrix() * transformMatrix;
		}

		return transformMatrix;
	}

	void Transform::SetParent(AEntity parent)
	{
		m_parent = parent;
	}

	bool Transform::operator==(const Transform& other) const
	{
		return position == other.position
			&& rotation == other.rotation && scale == other.scale;
	}

	bool Transform::operator!=(const Transform& other) const
	{
		return !(*this == other);
	}
}