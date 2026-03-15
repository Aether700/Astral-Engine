#include "aepch.h"
#include "CallbackComponentList.h"

namespace AstralEngine
{
	// CallbackList ////////////////////////////////////////////////////////////////////
	CallbackList::CallbackList() { }
	CallbackList::CallbackList(const CallbackList&) { }
	CallbackList::CallbackList(CallbackList&& other) noexcept : ComponentPairListBase(std::move(other)) { }

	void CallbackList::CallOnStart()
	{
		for (CallbackAEntityPair* callback : *this)
		{
			callback->OnStart();
		}
	}

	void CallbackList::CallOnUpdate()
	{
		for (CallbackAEntityPair* callback : *this)
		{
			callback->OnUpdate();
		}
	}

	void CallbackList::CallOnLateUpdate()
	{
		for (CallbackAEntityPair* callback : *this)
		{
			callback->OnLateUpdate();
		}
	}

	CallbackList& CallbackList::operator=(const CallbackList& other)
	{
		return (CallbackList&)AssignmentOperator(other);
	}

	CallbackList& CallbackList::operator=(CallbackList&& other) noexcept
	{
		return (CallbackList&)MoveOperator(std::move(other));
	}

	bool CallbackList::operator==(const CallbackList& other) const
	{
		return EqualOperator(other);
	}

	bool CallbackList::operator!=(const CallbackList& other) const
	{
		return !(*this == other);
	}
}