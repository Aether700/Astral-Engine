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
		ToggleableComponent(bool enabled = true) : m_enabled(enabled) { }

		virtual ~ToggleableComponent() { }

		virtual void OnEnable() { }
		virtual void OnDisable() { }

		bool IsEnabled() const { return m_enabled; }

		void SetEnable(bool val)
		{
			if (val == m_enabled)
			{
				return;
			}

			if (val)
			{
				m_enabled = true;
				OnEnable();
			}
			else
			{
				m_enabled = false;
				OnDisable();
			}
		}

	private:
		bool m_enabled;
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
			if (IsEnabled())
			{
				OnUpdate();
			}
		}

	};

	struct NameComponent
	{
		std::string name;

		NameComponent() : name("AEntity") { }

		bool operator==(const NameComponent& other) const
		{
			return name == other.name;
		}

		bool operator!=(const NameComponent& other) const
		{
			return !(*this == other);
		}

		operator std::string() const
		{
			return name;
		}
	};

	struct TagComponent
	{
		TagComponent() { }
		TagComponent(const std::string& str) : tag(str) { }

		bool operator==(const TagComponent& other) const
		{
			return tag == other.tag;
		}

		bool operator!=(const TagComponent& other) const
		{
			return !(*this == other);
		}

		std::string tag;
	};

	class SpriteRendererComponent : public ToggleableComponent
	{
	public:

		SpriteRendererComponent() 
			: m_color(1.0f, 1.0f, 1.0f, 1.0f), 
			m_sprite(Renderer2D::GetDefaultTexture()) { }
		
		SpriteRendererComponent(float r, float g, float b, float a) 
			: m_color(r, g, b, a), m_sprite(Renderer2D::GetDefaultTexture())
		{
			//make sure color provided is a valid color
			AE_CORE_ASSERT(r <= 1.0f && r >= 0.0f
					   &&  g <= 1.0f && g >= 0.0f
					   &&  b <= 1.0f && b >= 0.0f
					   &&  a <= 1.0f && a >= 0.0f, "Invalid Color provided");
		}

		SpriteRendererComponent(const Vector4& color) 
			: m_color(color), m_sprite(Renderer2D::GetDefaultTexture())
		{
			//make sure color provided is a valid color
			AE_CORE_ASSERT(color.r <= 1.0f && color.r >= 0.0f
				        && color.g <= 1.0f && color.g >= 0.0f
				        && color.b <= 1.0f && color.b >= 0.0f
				        && color.a <= 1.0f && color.a >= 0.0f, "Invalid Color provided");
		}

		SpriteRendererComponent(const AReference<Texture2D>& sprite)
			: m_color(1.0f, 1.0f, 1.0f, 1.0f), m_sprite(sprite)
		{
			AE_CORE_ASSERT(sprite != nullptr, "Invalid Sprite provided");
		}

		SpriteRendererComponent(float r, float g, float b, float a, const AReference<Texture2D>& sprite)
			: m_color(r, g, b, a), m_sprite(sprite)
		{
			AE_CORE_ASSERT(sprite != nullptr, "Invalid Sprite provided");
			//make sure color provided is a valid color
			AE_CORE_ASSERT(r <= 1.0f && r >= 0.0f
				&& g <= 1.0f && g >= 0.0f
				&& b <= 1.0f && b >= 0.0f
				&& a <= 1.0f && a >= 0.0f, "Invalid Color provided");
		}

		SpriteRendererComponent(const Vector4& color, const AReference<Texture2D>& sprite)
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

		bool operator==(const SpriteRendererComponent& other) const
		{
			return m_color == other.m_color && m_sprite == other.m_sprite;
		}

		bool operator!=(const SpriteRendererComponent& other) const
		{
			return !(*this == other);
		}

	private:
		Vector4 m_color;
		AReference<Texture2D> m_sprite;
	};

	class TransformComponent
	{
	public:

		TransformComponent() : scale(1.0f, 1.0f, 1.0f) { }
		TransformComponent(Vector3 translation) 
			: position(translation), scale(1.0f, 1.0f, 1.0f) { }

		TransformComponent(Vector3 pos, Vector3 rotation, Vector3 scale)
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

		bool operator==(const TransformComponent& other) const
		{
			return position == other.position
				&& rotation == other.rotation && scale == other.scale;
		}

		bool operator!=(const TransformComponent& other) const
		{
			return !(*this == other);
		}

		Vector3 position;
		Vector3 rotation;
		Vector3 scale;

	private:
		AReference<TransformComponent> m_parent;
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

		const std::string& GetName() const { return m_nameComp->name; }

		void SetName(const std::string& name) { m_nameComp->name = name; }

		static void Destroy(AEntity& e) { e.Destroy(); }

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
		TransformComponent* transform;

	private:
		void SetEntity(AEntity& e)
		{
			entity = e;
			transform = &entity.GetComponent<TransformComponent>();
			m_nameComp = &entity.GetComponent<NameComponent>();
			m_enabled = true;

			////the OnCreate function is called by AEntity not by the SignalHandler (leave it so it can be called later)
			//e.m_scene->AddCallbackOnCreate<AReference<NativeScript>>(function<&NativeScript::OnCreateCallback>, this);
			//e.m_scene->AddCallbackOnUpdate<AReference<NativeScript>>(function<&NativeScript::OnUpdateCallback>, this);
			//e.m_scene->AddCallbackOnDestroy<AReference<NativeScript>>(function<&NativeScript::OnDestroyCallback>, this);
		}
		
		NameComponent* m_nameComp;

		//callbacks used to pass to delegates to call the appropriated function 
		//even if overwritten by child class, the arguments are to be used by the Register
		void OnCreateCallback(Registry<BaseEntity>&, const BaseEntity id, const AReference<NativeScript>& script)
		{
			if (entity.GetID() == id && IsEnabled() && *this == *script)
			{
				OnCreate();
			}
		}

		void OnUpdateCallback(Registry<BaseEntity>&, const BaseEntity id, const AReference<NativeScript>& script)
		{
			if (entity.GetID() == id && IsEnabled() && *this == *script)
			{
				OnUpdate();
			}
		}

		void OnDestroyCallback(Registry<BaseEntity>&, const BaseEntity id, const AReference<NativeScript>& script)
		{
			if (entity.GetID() == id && IsEnabled() && *this == *script)
			{
				OnDestroy();
			}
		}

	};

}