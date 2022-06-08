#pragma once
#include <string>
#include "AstralEngine/Renderer/Renderer2D.h"
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
		SpriteRenderer(const AReference<Texture2D>& sprite);
		SpriteRenderer(float r, float g, float b, float a, const AReference<Texture2D>& sprite);
		SpriteRenderer(const Vector4& color, const AReference<Texture2D>& sprite);

		Vector4 GetColor() const { return m_color; }
		void SetColor(float r, float g, float b, float a);
		void SetColor(Vector4 color);

		AReference<Texture2D> GetSprite() const { return m_sprite; }
		void SetSprite(AReference<Texture2D> sprite);

		bool operator==(const SpriteRenderer& other) const;
		bool operator!=(const SpriteRenderer& other) const;

	private:
		Vector4 m_color;
		AReference<Texture2D> m_sprite;
	};

	class Transform
	{
	public:
		Transform();
		Transform(const Vector3& translation);
		Transform(const Vector3& pos, const Quaternion& rotation, const Vector3& scale);

		Mat4 GetTransformMatrix() const;

		AEntity GetParent() const { return m_parent; }
		void SetParent(AEntity parent);

		Vector3 Forward() const;
		Vector3 Right() const;
		Vector3 Up() const;

		bool operator==(const Transform& other) const;
		bool operator!=(const Transform& other) const;

		Vector3 position;
		Quaternion rotation;
		Vector3 scale;

	private:
		AEntity m_parent;
	};

	//Base class for any entity which needs to reference the AEntity object it is attached to
	class EntityLinkedComponent
	{
		friend class AEntity;
	public:
		~EntityLinkedComponent() { }

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

		template<typename... Component>
		bool HasComponent() const
		{
			return entity.HasComponent<Component...>();
		}

		template<typename Component>
		void RemoveComponent()
		{
			entity.RemoveComponent<Component>();
		}

		Transform& GetTransform() { return entity.GetTransform(); }
		const Transform& GetTransform() const { return entity.GetTransform(); }

		const std::string& GetName() const { return entity.GetName(); }
		void SetName(const std::string& name) { entity.SetName(name); }

		static void Destroy(AEntity& e) { e.Destroy(); }

	protected:
		AEntity entity;

	private:
		virtual void SetEntity(AEntity& e)
		{
			entity = e;
		}
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

	class NativeScript : public CallbackComponent, public EntityLinkedComponent
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

	private:
		void SetEntity(AEntity& e)
		{
			entity = e;
			m_isActive = true;
		}
	};
}