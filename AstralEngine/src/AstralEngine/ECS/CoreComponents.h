#pragma once

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

	//add all callback components to this list so they can easily be retrieved and their callbacks can be accessed easily
	class CallbackList
	{
	public:
		CallbackList() { }
		CallbackList(const CallbackList&) { }

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
}