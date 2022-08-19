#pragma once
#include <string>
#include "AstralEngine/Renderer/Texture.h"
#include "AstralEngine/Math/AMath.h"
#include "SceneCamera.h"
#include "CoreComponents.h"
#include "AEntity.h"

namespace AstralEngine
{
	class SpriteRenderer : public ToggleableComponent
	{
	public:
		SpriteRenderer();
		SpriteRenderer(float r, float g, float b, float a);
		SpriteRenderer(const Vector4& color);
		SpriteRenderer(Texture2DHandle sprite);
		SpriteRenderer(float r, float g, float b, float a, Texture2DHandle sprite);
		SpriteRenderer(const Vector4& color, Texture2DHandle sprite);

		Vector4 GetColor() const { return m_color; }
		void SetColor(float r, float g, float b, float a);
		void SetColor(Vector4 color);

		Texture2DHandle GetSprite() const { return m_sprite; }
		void SetSprite(Texture2DHandle sprite);

		bool operator==(const SpriteRenderer& other) const;
		bool operator!=(const SpriteRenderer& other) const;

	private:
		Vector4 m_color;
		Texture2DHandle m_sprite;
	};

	class Transform
	{
	public:
		Transform();
		Transform(const Vector3& translation);
		Transform(const Vector3& pos, const Quaternion& rotation, const Vector3& scale);
		Transform(const Vector3& pos, const Vector3& euler, const Vector3& scale);

		Mat4 GetTransformMatrix() const;

		const Vector3& GetLocalPosition() const;
		void SetLocalPosition(const Vector3& position);
		void SetLocalPosition(float x, float y, float z);

		const Quaternion& GetRotation() const;
		void SetRotation(const Quaternion& rotation);
		void SetRotation(const Vector3& euler);
		void SetRotation(float x, float y, float z);

		const Vector3& GetScale() const;
		void SetScale(const Vector3& scale);

		AEntity GetParent() const { return m_parent; }
		void SetParent(AEntity parent);

		void LookAt(const Transform& target, const Vector3& up = Vector3(0.0f, 1.0f, 0.0f));
		void LookAt(const Vector3& target, const Vector3& up = Vector3(0.0f, 1.0f, 0.0f));

		// rotates the transform around the point by the angle provided around the specified axis. 
		// Note that this function does affects both the internal rotation of the transform and it's position
		void RotateAround(const Vector3& point, float angle, const Vector3& axis = Vector3(0.0f, 1.0f, 0.0f));

		// returns true if the transform has changed since last frame, false otherwise
		bool HasChanged() const;

		Vector3 Forward() const;
		Vector3 Right() const;
		Vector3 Up() const;

		bool operator==(const Transform& other) const;
		bool operator!=(const Transform& other) const;


	private:
		Vector3 m_position;
		Quaternion m_rotation;
		Vector3 m_scale;

		AEntity m_parent;
		
		mutable Mat4 m_transformMatrix;
		mutable bool m_dirty;
		mutable bool m_hasChanged;
	};

	struct Camera : public ToggleableComponent
	{
		SceneCamera camera;
		bool primary = true;
		bool fixedAspectRatio = false;

		bool operator==(const Camera& other) const
		{
			return camera == other.camera && primary == other.primary 
				&& fixedAspectRatio == other.fixedAspectRatio;
		}

		bool operator!=(const Camera& other) const
		{
			return !(*this == other);
		}
	};

	class NativeScript : public CallbackComponent
	{
		friend class AEntity;
	public:

		NativeScript() : CallbackComponent(false) { }

		template<typename... Component>
		decltype(auto) GetComponent()
		{
			return entity.GetComponent<Component...>();
		}

		template<typename... Component>
		decltype(auto) GetComponent() const
		{
			return entity.GetComponent<Component...>();
		}

		Transform& GetTransform() { return entity.GetTransform(); }
		const Transform& GetTransform() const { return entity.GetTransform(); }

		const std::string& GetName() const { return entity.GetName(); }
		void SetName(const std::string& name) { entity.SetName(name); }

		void Destroy(AEntity& e) const { e.Destroy(); }
		AEntity CreateAEntity() const { return entity.m_scene->CreateAEntity(); }

		bool operator==(const NativeScript& other) const
		{
			return entity == other.entity;
		}

		bool operator!=(const NativeScript& other) const
		{
			return !(*this == other);
		}

	protected:
		AEntity entity;

	private:
		void SetEntity(AEntity& e)
		{
			entity = e;
			m_isActive = true;
		}
	};
}