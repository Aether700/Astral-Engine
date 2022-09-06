#include "aepch.h"
#include "Components.h"
#include "AstralEngine/Renderer/Renderer.h"
#include "AstralEngine/Renderer/RendererInternals.h"

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

	void SpriteRenderer::SendDataToRenderer(const Transform& transform) const
	{
		Renderer::DrawSprite(transform, *this);
	}

	// MeshRenderer ///////////////////////////////////////////////////

	MeshRenderer::MeshRenderer() : m_mesh(NullHandle), m_material(Material::DefaultMat()) { }
	MeshRenderer::MeshRenderer(MeshHandle mesh) : m_mesh(mesh), m_material(Material::DefaultMat()) { }
	MeshRenderer::MeshRenderer(MeshHandle mesh, MaterialHandle mat) : m_mesh(mesh), m_material(mat) { }

	MeshHandle MeshRenderer::GetMesh() const { return m_mesh; }
	void MeshRenderer::SetMesh(MeshHandle mesh) { m_mesh = mesh; }

	MaterialHandle MeshRenderer::GetMaterial() const { return m_material; }

	void MeshRenderer::SetMaterial(MaterialHandle mat) { m_material = mat; }

	bool MeshRenderer::operator==(const MeshRenderer& other) const
	{
		return m_mesh == other.m_mesh;
	}

	bool MeshRenderer::operator!=(const MeshRenderer& other) const
	{
		return !(*this == other);
	}

	void MeshRenderer::SendDataToRenderer(const Transform& transform) const
	{
		Renderer::DrawMesh(transform, *this);
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

	// Light ///////////////////////////////////////////////////
	Light::Light() : m_light(NullHandle), m_type(LightType::Directional), m_direction(-0.5f, -1.0f, -0.5f), 
		m_color(1.0f, 1.0f, 1.0f)
	{
		m_direction.Normalize();
	}
	
	void Light::OnStart()
	{
		Renderer::RemoveLight(m_light);
		m_light = Renderer::AddLight(CreateLightData());
	}

	void Light::OnEnable()
	{
		m_light = Renderer::AddLight(CreateLightData());
	}

	void Light::OnDisable()
	{
		Renderer::RemoveLight(m_light);
	}

	LightHandle Light::GetHandle() const { return m_light; }

	LightType Light::GetType() const
	{
		LightData& data = RetrieveLightData();
		return data.GetLightType();
	}

	const Vector3& Light::GetColor() const
	{
		LightData& data = RetrieveLightData();
		return data.GetColor();
	}

	const Vector3& Light::GetDirection() const
	{
		LightData& data = RetrieveLightData();
		return data.GetDirection();
	}

	float Light::GetRadius() const { return m_radius; }

	void Light::SetType(LightType type)
	{
		if (m_type != type)
		{
			if (Renderer::LightIsValid(m_light))
			{
				LightData& data = RetrieveLightData();
				data.SetLightType(type);
				Renderer::s_lightHandler.OnLightTypeChange(m_light, m_type, type);
			}
			m_type = type;
		}
	}

	void Light::SetColor(const Vector3& color)
	{
		LightData& data = RetrieveLightData();
		data.SetColor(color);
		m_color = color;
	}
	
	void Light::SetDirection(const Vector3& direction)
	{
		Vector3 normalizedDir = Vector3::Normalize(direction);
		LightData& data = RetrieveLightData();
		data.SetDirection(normalizedDir);
		m_direction = normalizedDir;
	}

	void Light::SetRadius(float radius)
	{
		m_radius = radius;
	}

	bool Light::operator==(const Light& other) const { return m_light == other.m_light; }
	bool Light::operator!=(const Light& other) const { return !(*this == other); }

	LightData& Light::RetrieveLightData() const
	{
		AE_CORE_ASSERT(Renderer::LightIsValid(m_light), "");
		return Renderer::GetLightData(m_light);
	}

	LightData Light::CreateLightData() const
	{
		LightData data = LightData(GetTransform().GetLocalPosition(), m_color);
		data.SetDirection(m_direction);
		data.SetLightType(m_type);
		data.SetRadius(m_radius);
		return data;
	}
}