#pragma once
#include "AstralEngine/ECS/ECS Core/ECSUtils.h"

namespace AstralEngine
{
	class AEntity;

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

	class CallbackComponent : public ToggleableComponent
	{
	public:
		CallbackComponent(bool enabled = true) : ToggleableComponent(enabled) { }
		virtual ~CallbackComponent() { }

		virtual void OnCreate() { }
		virtual void OnStart() { }
		virtual void OnUpdate() { }
		virtual void OnDestroy() { }

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

	// base object used to gather pairs of AEntity objects and different types 
	//of CallbackComponent objects
	class CallbackAEntityPair
	{
	public:
		virtual ~CallbackAEntityPair() { }
		virtual void OnStart() = 0;
		virtual void OnUpdate() = 0;
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
		CallbackList() { }
		CallbackList(const CallbackList&) { }
		CallbackList(CallbackList&& other) 
		{
			m_callbacks = std::move(other.m_callbacks);
		}

		~CallbackList()
		{
			Clear();
		}


		void AddCallback(CallbackAEntityPair* callback)
		{
			m_callbacks.Add(callback);
		}

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

		void CallOnStart()
		{
			for (CallbackAEntityPair* callback : m_callbacks)
			{
				callback->OnStart();
			}
		}

		void CallOnUpdate()
		{
			for (CallbackAEntityPair* callback : m_callbacks)
			{
				callback->OnUpdate();
			}
		}

		bool IsEmpty() const { return m_callbacks.IsEmpty(); }

		void Clear()
		{
			for (CallbackAEntityPair* ptr : m_callbacks)
			{
				delete ptr;
			}
			m_callbacks.Clear();
		}

		CallbackList& operator=(const CallbackList& other)
		{
			Clear();
			m_callbacks = other.m_callbacks;
			return *this;
		}

		CallbackList& operator=(CallbackList&& other)
		{
			Clear();
			m_callbacks = std::move(other.m_callbacks);
			return *this;
		}

		bool operator==(const CallbackList& other) const
		{
			return m_callbacks == other.m_callbacks;
		}

		bool operator!=(const CallbackList& other) const
		{
			return !(*this == other);
		}

	private:
		ASinglyLinkedList<CallbackAEntityPair*> m_callbacks;
	};
}