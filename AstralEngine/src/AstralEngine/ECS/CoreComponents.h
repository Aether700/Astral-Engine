#pragma once
#include "AstralEngine/ECS/ECS Core/ECSUtils.h"

namespace AstralEngine
{
	class AEntity;
	class Renderable;
	class Transform;
	class ComponentPairBase;

	class ToggleableComponent
	{
		friend class NativeScript;
	public:
		ToggleableComponent(bool enabled = true);

		virtual ~ToggleableComponent();

		virtual void OnEnable();
		virtual void OnDisable();

		bool IsActive() const;
		void SetActive(bool val);

	private:
		bool m_isActive;
	};

	class AEntityRenderablePair
	{
	public:
		virtual ~AEntityRenderablePair();
		
		virtual void SendToRenderer(const Transform& transform) const = 0;
		virtual bool IsActive() const = 0;
	};

	template<typename Component>
	class AEntityRenderableComponentPair : public AEntityRenderablePair
	{
	public:
		AEntityRenderableComponentPair(AEntity e) : m_entity(e) { }
		virtual void SendToRenderer(const Transform& transform) const override
		{
			m_entity.GetComponent<Component>().SendDataToRenderer(transform);
		}

		virtual bool IsActive() const override
		{
			return m_entity.GetComponent<Component>().IsActive();
		}

	private:
		AEntity m_entity;
	};

	class RenderData sealed
	{
	public:
		RenderData();
		RenderData(AEntityRenderablePair* r);
		RenderData(RenderData&& other) noexcept;
		~RenderData();

		void SendToRenderer(const Transform& transform) const;
		bool IsActive() const;

		RenderData& operator=(RenderData&& other) noexcept;
		bool operator==(const RenderData& other) const;
		bool operator!=(const RenderData& other) const;

	private:
		AEntityRenderablePair* m_renderable;
	};

	class CallbackComponent : public ToggleableComponent
	{
	public:
		CallbackComponent(bool enabled = true);
		virtual ~CallbackComponent();

		virtual void OnCreate();
		virtual void OnStart();
		virtual void OnUpdate();
		virtual void OnLateUpdate();
		virtual void OnDestroy();

		bool operator==(const CallbackComponent& other) const;
		bool operator!=(const CallbackComponent& other) const;
	};

	class AEntityData sealed : public ToggleableComponent
	{
	public:
		AEntityData();

		const std::string& GetName() const;
		const std::string& GetTag() const;

		void SetName(const std::string& name);
		void SetTag(const std::string& tag);

		bool operator==(const AEntityData& other) const;
		bool operator!=(const AEntityData& other) const;

	private:
		std::string m_name;
		std::string m_tag;
	};
}