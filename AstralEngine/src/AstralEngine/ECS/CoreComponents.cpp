#include "aepch.h"
#include "Components.h"
#include "CoreComponents.h"
#include "AEntity.h"

namespace AstralEngine
{
	// ToggleableComponent //////////////////////////////////////////////////////////
	ToggleableComponent::ToggleableComponent(bool enabled) : m_isActive(enabled) { }

	ToggleableComponent::~ToggleableComponent() { }

	void ToggleableComponent::OnEnable() { }
	void ToggleableComponent::OnDisable() { }

	bool ToggleableComponent::IsActive() const { return m_isActive; }

	void ToggleableComponent::SetActive(bool val)
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

	// AEntityRenderablePair /////////////////////////////////////////////
	AEntityRenderablePair::~AEntityRenderablePair() { }

	// RenderData ////////////////////////////////////////
	RenderData::RenderData() : m_renderable(nullptr) { }
	RenderData::RenderData(AEntityRenderablePair* r) : m_renderable(r) { }
	RenderData::RenderData(RenderData&& other) noexcept : m_renderable(other.m_renderable) 
	{
		other.m_renderable = nullptr;
	}

	RenderData::~RenderData() { delete m_renderable; }

	void RenderData::SendToRenderer(const Transform& transform) const
	{
		m_renderable->SendToRenderer(transform);
	}

	bool RenderData::IsActive() const
	{
		return m_renderable->IsActive();
	}

	RenderData& RenderData::operator=(RenderData&& other) noexcept
	{
		delete m_renderable;
		m_renderable = other.m_renderable;
		other.m_renderable = nullptr;
		return *this;
	}

	bool RenderData::operator==(const RenderData& other) const { return m_renderable == other.m_renderable; }
	bool RenderData::operator!=(const RenderData& other) const { return !(*this == other); }


	// CallbackComponent ///////////////////////////////////////////////////////
	CallbackComponent::CallbackComponent(bool enabled) : ToggleableComponent(enabled) { }
	CallbackComponent::~CallbackComponent() { }

	void CallbackComponent::OnCreate() { }
	void CallbackComponent::OnStart() { }
	void CallbackComponent::OnUpdate() { }
	void CallbackComponent::OnLateUpdate() { }
	void CallbackComponent::OnDestroy() { }

	bool CallbackComponent::operator==(const CallbackComponent& other) const
	{
		return this == &other;
	}

	bool CallbackComponent::operator!=(const CallbackComponent& other) const
	{
		return !(*this == other);
	}

	// AEntityData /////////////////////////////////////////////////////////
	AEntityData::AEntityData() : m_name("AEntity"), m_tag("Untagged") { }

	const std::string& AEntityData::GetName() const { return m_name; }
	const std::string& AEntityData::GetTag() const { return m_tag; }

	void AEntityData::SetName(const std::string& name) { m_name = name; }
	void AEntityData::SetTag(const std::string& tag) { m_tag = tag; }

	bool AEntityData::operator==(const AEntityData& other) const
	{
		return IsActive() == other.IsActive() && m_name == other.m_name 
			&& m_tag == other.m_tag;
	}

	bool AEntityData::operator!=(const AEntityData& other) const
	{
		return !(*this == other);
	}

	// CallbackList ///////////////////////////////////////////////////
	CallbackList::CallbackList() { }
	CallbackList::CallbackList(const CallbackList&) { }
	CallbackList::CallbackList(CallbackList&& other) noexcept
	{
		m_callbacks = std::move(other.m_callbacks);
	}

	CallbackList::~CallbackList()
	{
		Clear();
	}


	void CallbackList::AddCallback(CallbackAEntityPair* callback)
	{
		m_callbacks.Add(callback);
	}

	void CallbackList::CallOnStart()
	{
		for (CallbackAEntityPair* callback : m_callbacks)
		{
			callback->OnStart();
		}
	}

	void CallbackList::CallOnUpdate()
	{
		for (CallbackAEntityPair* callback : m_callbacks)
		{
			callback->OnUpdate();
		}
	}

	void CallbackList::CallOnLateUpdate()
	{
		for (CallbackAEntityPair* callback : m_callbacks)
		{
			callback->OnLateUpdate();
		}
	}

	bool CallbackList::IsEmpty() const { return m_callbacks.IsEmpty(); }

	void CallbackList::Clear()
	{
		for (CallbackAEntityPair* ptr : m_callbacks)
		{
			delete ptr;
		}
		m_callbacks.Clear();
	}

	CallbackList& CallbackList::operator=(const CallbackList& other)
	{
		Clear();
		m_callbacks = other.m_callbacks;
		return *this;
	}

	CallbackList& CallbackList::operator=(CallbackList&& other) noexcept
	{
		Clear();
		m_callbacks = std::move(other.m_callbacks);
		return *this;
	}

	bool CallbackList::operator==(const CallbackList& other) const
	{
		return m_callbacks == other.m_callbacks;
	}

	bool CallbackList::operator!=(const CallbackList& other) const
	{
		return !(*this == other);
	}
}