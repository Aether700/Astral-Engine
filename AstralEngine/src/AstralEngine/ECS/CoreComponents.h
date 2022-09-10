#pragma once
#include "AstralEngine/ECS/ECS Core/ECSUtils.h"

namespace AstralEngine
{
	class AEntity;
	class Renderable;
	class Transform;

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

	// base object used to gather pairs of AEntity objects and different types 
	//of CallbackComponent objects
	class CallbackAEntityPair
	{
	public:
		virtual ~CallbackAEntityPair() { }
		virtual void OnStart() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnLateUpdate() = 0;
		virtual size_t GetTypeID() const = 0;
	};

	template<typename Component>
	class ComponentAEntityPair : public CallbackAEntityPair
	{
	public:
		ComponentAEntityPair(AEntity e) : m_entity(e) { }

		virtual void OnStart() override
		{
			if (AEntityAndComponentAreActive())
			{
				m_entity.GetComponent<Component>().OnStart();
			}
		}

		virtual void OnUpdate() override
		{
			if (AEntityAndComponentAreActive())
			{
				m_entity.GetComponent<Component>().OnUpdate();
			}
		}

		virtual void OnLateUpdate() override
		{
			if (AEntityAndComponentAreActive())
			{
				m_entity.GetComponent<Component>().OnLateUpdate();
			}
		}

		virtual size_t GetTypeID() const override
		{
			return TypeInfo<Component>::ID();
		}

	private:
		bool AEntityAndComponentAreActive()
		{
			if (m_entity.IsActive())
			{
				Component& comp = m_entity.GetComponent<Component>();
				return comp.IsActive();
			}
			return false;
		}

		AEntity m_entity;
	};

	//add all callback components to this list so they can easily be retrieved and their callbacks can be accessed easily
	class CallbackList
	{
	public:
		CallbackList();
		CallbackList(const CallbackList&);
		CallbackList(CallbackList&& other) noexcept;

		~CallbackList();


		void AddCallback(CallbackAEntityPair* callback);

		template<typename Component>
		void RemoveCallback()
		{
			size_t id = TypeInfo<Component>::ID();
			for (auto it = m_callbacks.begin(); it != m_callbacks.end(); it++)
			{
				if ((*it)->GetTypeID() == id)
				{
					CallbackAEntityPair* removed = *it;
					m_callbacks.Remove(it);
					delete removed;
					return;
				}
			}
		}

		void CallOnStart();
		void CallOnUpdate();
		void CallOnLateUpdate();

		bool IsEmpty() const;
		void Clear();

		CallbackList& operator=(const CallbackList& other);
		CallbackList& operator=(CallbackList&& other) noexcept;
		bool operator==(const CallbackList& other) const;
		bool operator!=(const CallbackList& other) const;

	private:
		ASinglyLinkedList<CallbackAEntityPair*> m_callbacks;
	};
}