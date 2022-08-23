#include "aepch.h"
#include "Components.h"
#include "AstralEngine/Renderer/Renderer.h"

namespace AstralEngine
{
	// SpriteRenderer ///////////////////////////////////////////////////
	SpriteRenderer::SpriteRenderer()
		: m_color(1.0f, 1.0f, 1.0f, 1.0f),
		m_sprite(Texture2D::WhiteTexture()) { }

	SpriteRenderer::SpriteRenderer(float r, float g, float b, float a)
		: m_color(r, g, b, a), m_sprite(Texture2D::WhiteTexture())
	{
		//make sure color provided is a valid color
		AE_CORE_ASSERT(r <= 1.0f && r >= 0.0f
			&& g <= 1.0f && g >= 0.0f
			&& b <= 1.0f && b >= 0.0f
			&& a <= 1.0f && a >= 0.0f, "Invalid Color provided");
	}

	SpriteRenderer::SpriteRenderer(const Vector4& color)
		: m_color(color), m_sprite(Texture2D::WhiteTexture())
	{
		//make sure color provided is a valid color
		AE_CORE_ASSERT(color.r <= 1.0f && color.r >= 0.0f
			&& color.g <= 1.0f && color.g >= 0.0f
			&& color.b <= 1.0f && color.b >= 0.0f
			&& color.a <= 1.0f && color.a >= 0.0f, "Invalid Color provided");
	}

	SpriteRenderer::SpriteRenderer(Texture2DHandle sprite)
		: m_color(1.0f, 1.0f, 1.0f, 1.0f), m_sprite(sprite)
	{
		if (sprite == NullHandle)
		{
			m_sprite = Texture2D::WhiteTexture();
		}
	}

	SpriteRenderer::SpriteRenderer(float r, float g, float b, float a, Texture2DHandle sprite)
		: m_color(r, g, b, a), m_sprite(sprite)
	{
		if (sprite == NullHandle)
		{
			m_sprite = Texture2D::WhiteTexture();
		}

		//make sure color provided is a valid color
		AE_CORE_ASSERT(r <= 1.0f && r >= 0.0f
			&& g <= 1.0f && g >= 0.0f
			&& b <= 1.0f && b >= 0.0f
			&& a <= 1.0f && a >= 0.0f, "Invalid Color provided");
	}

	SpriteRenderer::SpriteRenderer(const Vector4& color, Texture2DHandle sprite)
		: m_color(color), m_sprite(sprite)
	{
		if (sprite == NullHandle)
		{
			m_sprite = Texture2D::WhiteTexture();
		}

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

	void SpriteRenderer::SetSprite(Texture2DHandle sprite)
	{
		if (sprite == NullHandle)
		{
			m_sprite = Texture2D::WhiteTexture();
		}
		else
		{
			m_sprite = sprite;
		}
	}

	bool SpriteRenderer::operator==(const SpriteRenderer& other) const
	{
		return m_color == other.m_color && m_sprite == other.m_sprite;
	}

	bool SpriteRenderer::operator!=(const SpriteRenderer& other) const
	{
		return !(*this == other);
	}

	// MeshRenderer ///////////////////////////////////////////////////

	MeshRenderer::MeshRenderer() : m_mesh(NullHandle), m_color(1, 1, 1, 1), m_material(Material::DefaultMat()) { }
	MeshRenderer::MeshRenderer(MeshHandle mesh, const Vector4& color) : m_mesh(mesh),
		m_color(1, 1, 1, 1), m_material(Material::DefaultMat()) { }
	MeshRenderer::MeshRenderer(MeshHandle mesh, MaterialHandle mat, const Vector4& color) : m_mesh(mesh), 
		m_color(1, 1, 1, 1), m_material(mat) { }

	MeshHandle MeshRenderer::GetMesh() const { return m_mesh; }
	void MeshRenderer::SetMesh(MeshHandle mesh) { m_mesh = mesh; }

	const Vector4& MeshRenderer::GetColor() const { return m_color; }
	void MeshRenderer::SetColor(const Vector4& color) { m_color = color; }
	
	void MeshRenderer::SetColor(float r, float g, float b, float a)
	{
		Vector4 color = Vector4(r, g, b, a);
		SetColor(color);
	}

	MaterialHandle MeshRenderer::GetMaterial() const { return m_material; }

	void MeshRenderer::SetMaterial(MaterialHandle mat) { m_material = mat; }

	bool MeshRenderer::operator==(const MeshRenderer& other) const
	{
		return m_mesh == other.m_mesh && m_color == other.m_color;
	}

	bool MeshRenderer::operator!=(const MeshRenderer& other) const
	{
		return !(*this == other);
	}


	// Transform //////////////////////////////////////////////////////

	Transform::Transform() : m_scale(1.0f, 1.0f, 1.0f), m_dirty(true), m_hasChanged(false) { }
	Transform::Transform(const Vector3& translation)
		: m_position(translation), m_scale(1.0f, 1.0f, 1.0f), m_dirty(true), m_hasChanged(false) { }

	Transform::Transform(const Vector3& pos, const Quaternion& rotation, const Vector3& scale)
		: m_position(pos), m_rotation(rotation), m_scale(scale), m_dirty(true), m_hasChanged(false) { }

	Transform::Transform(const Vector3& pos, const Vector3& euler, const Vector3& scale)
		: m_position(pos), m_rotation(euler), m_scale(scale), m_dirty(true), m_hasChanged(false) { }


	Mat4 Transform::GetTransformMatrix() const
	{
		bool parentDirty = false;
		if (m_parent != NullEntity)
		{
			const Transform& parent = m_parent.GetTransform();
			parentDirty = parent.m_dirty || parent.HasChanged();
		}

		if (m_dirty || parentDirty)
		{
			m_hasChanged = true;
			m_dirty = false;
			if (m_rotation == Quaternion::Identity())
			{
				m_transformMatrix = Mat4::Translate(Mat4::Identity(), m_position) * Mat4::Scale(Mat4::Identity(), m_scale);
			}
			else
			{
				Mat4 rotationMatrix = m_rotation.ComputeRotationMatrix();
				m_transformMatrix = Mat4::Translate(Mat4::Identity(), m_position)
					* rotationMatrix * Mat4::Scale(Mat4::Identity(), m_scale);
			}

			if (m_parent.IsValid())
			{
				m_transformMatrix = m_parent.GetTransform().GetTransformMatrix() * m_transformMatrix;
			}
		}
		return m_transformMatrix;
	}

	const Vector3& Transform::GetLocalPosition() const { return m_position; }
	
	void Transform::SetLocalPosition(const Vector3& position)
	{ 
		m_position = position;
		m_dirty = true;
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{
		m_position = Vector3(x, y, z);
		m_dirty = true;
	}

	const Quaternion& Transform::GetRotation() const { return m_rotation; }

	void Transform::SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
		m_dirty = true;
	}

	void Transform::SetRotation(const Vector3& euler)
	{
		m_rotation.SetEulerAngles(euler);
		m_dirty = true;
	}

	void Transform::SetRotation(float x, float y, float z)
	{
		m_rotation.SetEulerAngles(x, y, z);
		m_dirty = true;
	}

	const Vector3& Transform::GetScale() const { return m_scale; }
	
	void Transform::SetScale(const Vector3& scale)
	{
		m_scale = scale;
		m_dirty = true;
	}

	void Transform::SetParent(AEntity parent)
	{
		m_parent = parent;
		m_dirty = true;
	}

	void Transform::LookAt(const Transform& target, const Vector3& up)
	{
		LookAt(target.m_position, up);
	}

	void Transform::LookAt(const Vector3& target, const Vector3& up)
	{
		m_rotation = Quaternion::LookRotation(target - m_position, up);
		m_dirty = true;
	}

	void Transform::RotateAround(const Vector3& point, float angle, const Vector3& axis)
	{
		Vector3 pivotSpacePos = m_position - point;
		Quaternion rot = Quaternion::AngleAxisToQuaternion(angle, axis);
		Vector3 resultPosPivotSpace = rot * pivotSpacePos;
		m_rotation = rot * m_rotation;
		m_position = resultPosPivotSpace + point;
		m_dirty = true;
	}

	bool Transform::HasChanged() const 
	{
		if (m_parent != NullEntity)
		{
			return m_hasChanged || m_parent.GetTransform().HasChanged();
		}
		return m_hasChanged; 
	}

	Vector3 Transform::Forward() const
	{
		return m_rotation * Vector3::Forward();
	}

	Vector3 Transform::Right() const
	{
		return m_rotation * Vector3::Right();
	}

	Vector3 Transform::Up() const
	{
		return m_rotation * Vector3::Up();
	}

	bool Transform::operator==(const Transform& other) const
	{
		return m_position == other.m_position
			&& m_rotation == other.m_rotation && m_scale == other.m_scale && m_parent == other.m_parent;
	}

	bool Transform::operator!=(const Transform& other) const
	{
		return !(*this == other);
	}
}