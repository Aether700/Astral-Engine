#pragma once
#include <string>
#include "AstralEngine/Renderer/Renderer2D.h"
#include "AstralEngine/Renderer/Texture.h"
#include "AstralEngine/Math/AMath.h"
#include "SceneCamera.h"
#include "AEntity.h"

namespace AstralEngine
{
	class ToggleableComponent
	{
		friend class NativeScript;
	public:
		ToggleableComponent(bool enabled = true) : m_isActive(enabled) { }

		virtual ~ToggleableComponent() { }

		virtual void OnEnable() { }
		virtual void OnDisable() { }

		bool IsActive() const { return m_isActive; }

		void SetActive(bool val)
		{
			if (val == m_isActive)
			{
				return;
			}

			if (val)
			{
				m_isActive = true;
				OnEnable();
			}
			else
			{
				m_isActive = false;
				OnDisable();
			}
		}

	private:
		bool m_isActive;
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

	class CallbackComponent : public ToggleableComponent
	{
	public:
		CallbackComponent(bool enabled = true) : ToggleableComponent(enabled) { }
		virtual ~CallbackComponent() { }

		virtual void OnCreate() { }
		virtual void OnStart() { }
		virtual void OnUpdate() { }
		virtual void OnDestroy() { }

		//calls OnUpdate but only if the component is enabled
		void FilteredUpdate()
		{
			if (IsActive())
			{
				OnUpdate();
			}
		}

		bool operator==(const CallbackComponent& other) const
		{
			return this == &other;
		}

		bool operator!=(const CallbackComponent& other) const
		{
			return !(*this == other);
		}
	};

	class AEntityData : public ToggleableComponent
	{
	public:
		AEntityData() : m_name("AEntity"), m_tag("Untagged") { }

		const std::string& GetName() const { return m_name; }
		const std::string& GetTag() const { return m_tag; }

		void SetName(const std::string& name) { m_name = name; }
		void SetTag(const std::string& tag) { m_tag = tag; }

		bool operator==(const AEntityData& other) const
		{
			return m_name == other.m_name && m_tag == other.m_tag 
				&& IsActive() == other.IsActive();
		}
		
		bool operator!=(const AEntityData& other) const
		{
			return !(*this == other);
		}

	private:
		std::string m_name;
		std::string m_tag;
	};

	class SpriteRenderer : public ToggleableComponent
	{
	public:
		SpriteRenderer()
			: m_color(1.0f, 1.0f, 1.0f, 1.0f), 
			m_sprite(Renderer2D::GetDefaultTexture()) { }
		
		SpriteRenderer(float r, float g, float b, float a)
			: m_color(r, g, b, a), m_sprite(Renderer2D::GetDefaultTexture())
		{
			//make sure color provided is a valid color
			AE_CORE_ASSERT(r <= 1.0f && r >= 0.0f
					   &&  g <= 1.0f && g >= 0.0f
					   &&  b <= 1.0f && b >= 0.0f
					   &&  a <= 1.0f && a >= 0.0f, "Invalid Color provided");
		}

		SpriteRenderer(const Vector4& color)
			: m_color(color), m_sprite(Renderer2D::GetDefaultTexture())
		{
			//make sure color provided is a valid color
			AE_CORE_ASSERT(color.r <= 1.0f && color.r >= 0.0f
				        && color.g <= 1.0f && color.g >= 0.0f
				        && color.b <= 1.0f && color.b >= 0.0f
				        && color.a <= 1.0f && color.a >= 0.0f, "Invalid Color provided");
		}

		SpriteRenderer(const AReference<Texture2D>& sprite)
			: m_color(1.0f, 1.0f, 1.0f, 1.0f), m_sprite(sprite)
		{
			AE_CORE_ASSERT(sprite != nullptr, "Invalid Sprite provided");
		}

		SpriteRenderer(float r, float g, float b, float a, const AReference<Texture2D>& sprite)
			: m_color(r, g, b, a), m_sprite(sprite)
		{
			AE_CORE_ASSERT(sprite != nullptr, "Invalid Sprite provided");
			//make sure color provided is a valid color
			AE_CORE_ASSERT(r <= 1.0f && r >= 0.0f
				&& g <= 1.0f && g >= 0.0f
				&& b <= 1.0f && b >= 0.0f
				&& a <= 1.0f && a >= 0.0f, "Invalid Color provided");
		}

		SpriteRenderer(const Vector4& color, const AReference<Texture2D>& sprite)
			: m_color(color), m_sprite(sprite)
		{
			AE_CORE_ASSERT(sprite != nullptr, "Invalid Sprite provided");
			//make sure color provided is a valid color
			AE_CORE_ASSERT(color.r <= 1.0f && color.r >= 0.0f
				&& color.g <= 1.0f && color.g >= 0.0f
				&& color.b <= 1.0f && color.b >= 0.0f
				&& color.a <= 1.0f && color.a >= 0.0f, "Invalid Color provided");
		}

		Vector4 GetColor() const { return m_color; }

		void SetColor(float r, float g, float b, float a) 
		{
			//make sure color provided is a valid color
			AE_CORE_ASSERT(r <= 1.0f && r >= 0.0f
				&& g <= 1.0f && g >= 0.0f
				&& b <= 1.0f && b >= 0.0f
				&& a <= 1.0f && a >= 0.0f, "Invalid Color provided");

			m_color = Vector4(r, g, b, a); 
		}

		void SetColor(Vector4 color) 
		{
			//make sure color provided is a valid color
			AE_CORE_ASSERT(color.r <= 1.0f && color.r >= 0.0f
				        && color.g <= 1.0f && color.g >= 0.0f
				        && color.b <= 1.0f && color.b >= 0.0f
				        && color.a <= 1.0f && color.a >= 0.0f, "Invalid Color provided");

			m_color = color; 
		}

		AReference<Texture2D> GetSprite() const { return m_sprite; }

		void SetSprite(AReference<Texture2D> sprite) 
		{
			AE_CORE_ASSERT(sprite != nullptr, "Invalid Sprite provided");
			m_sprite = sprite;
		}

		bool operator==(const SpriteRenderer& other) const
		{
			return m_color == other.m_color && m_sprite == other.m_sprite;
		}

		bool operator!=(const SpriteRenderer& other) const
		{
			return !(*this == other);
		}

	private:
		Vector4 m_color;
		AReference<Texture2D> m_sprite;
	};

	class Transform
	{
	public:
		Transform() : scale(1.0f, 1.0f, 1.0f) { }
		Transform(Vector3 translation)
			: position(translation), scale(1.0f, 1.0f, 1.0f) { }

		Transform(Vector3 pos, Vector3 rotation, Vector3 scale)
			: position(pos), rotation(rotation), scale(scale) { }


		Mat4 GetTransformMatrix() const
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

			if (m_parent != nullptr)
			{
				return m_parent->GetTransformMatrix() * transformMatrix;
			}

			return transformMatrix;
		}

		bool operator==(const Transform& other) const
		{
			return position == other.position
				&& rotation == other.rotation && scale == other.scale;
		}

		bool operator!=(const Transform& other) const
		{
			return !(*this == other);
		}

		Vector3 position;
		Vector3 rotation; //rotation is in degree
		Vector3 scale;

	private:
		AReference<Transform> m_parent;
	};

	struct CameraComponent : public ToggleableComponent
	{
		SceneCamera camera;
		bool primary = true;
		bool fixedAspectRatio = false;

		bool operator==(const CameraComponent& other) const
		{
			return camera == other.camera && primary == other.primary 
				&& fixedAspectRatio == other.fixedAspectRatio;
		}

		bool operator!=(const CameraComponent& other) const
		{
			return !(*this == other);
		}
	};

	//add all callback components to this list so they can easily be retrieved and their callbacks can be accessed easily
	class CallbackList
	{
	public:
		void AddCallback(CallbackComponent* callback)
		{
			m_callbacks.Add(callback);
		}

		void RemoveCallback(CallbackComponent* callback)
		{
			m_callbacks.Remove(callback);
		}

		void CallOnStart()
		{
			for (CallbackComponent* callback : m_callbacks) 
			{
				callback->OnStart();
			}
		}

		void CallOnUpdate()
		{
			for (CallbackComponent* callback : m_callbacks)
			{
				callback->FilteredUpdate();
			}
		}

		bool IsEmpty() const { return m_callbacks.IsEmpty(); }

		bool operator==(const CallbackList& other) const
		{
			return m_callbacks == other.m_callbacks;
		}

		bool operator!=(const CallbackList& other) const
		{
			return !(*this == other);
		}

	private:
		ASinglyLinkedList<CallbackComponent*> m_callbacks;
	};

	class NativeScript : public CallbackComponent, public EntityLinkedComponent
	{
		friend class AEntity;
	public:
		NativeScript() : CallbackComponent(false) { }

		bool operator==(const NativeScript& other) const
		{
			return entity == other.entity;
		}

		bool operator!=(const NativeScript& other) const
		{
			return !(*this == other);
		}

	private:
		virtual void SetEntity(AEntity& e) override
		{
			entity = e;
			m_isActive = true;
		}
	};

}